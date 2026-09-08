#include "source-record-status.h"
#include <QApplication>
#include <QDockWidget>
#include <QLabel>
#include <QMainWindow>
#include <QMetaObject>
#include <QPointer>
#include <QVBoxLayout>
#include <QWidget>
#include <map>
#include <string>
#include <obs-frontend-api.h>

namespace {
struct StatusEntry { std::string name; bool active = false; bool paused = false; };
QPointer<QDockWidget> dock;
QPointer<QLabel> statusLabel;
std::map<void *, StatusEntry> entries;

static void refresh()
{
    if (!statusLabel) return;
    size_t active_count = 0, paused_count = 0;
    for (const auto &pair : entries) {
        const StatusEntry &entry = pair.second;
        if (!entry.active) continue;
        ++active_count;
        if (entry.paused) ++paused_count;
    }

    QString text, style;
    if (active_count == 0) {
        text = QStringLiteral("●  IDLE");
        style = QStringLiteral("color:#888888;font-size:16px;font-weight:600;");
    } else if (paused_count == active_count) {
        text = QStringLiteral("Ⅱ  PAUSED");
        if (active_count > 1) text += QStringLiteral(" (%1)").arg((qulonglong)active_count);
        style = QStringLiteral("color:#d8a400;font-size:16px;font-weight:700;");
    } else {
        text = QStringLiteral("●  RECORDING");
        if (active_count > 1) text += QStringLiteral(" (%1)").arg((qulonglong)active_count);
        style = QStringLiteral("color:#e04b4b;font-size:16px;font-weight:700;");
    }
    statusLabel->setText(text);
    statusLabel->setStyleSheet(style);
}

static void update_impl(void *context, const char *name, bool active, bool paused)
{
    entries[context] = {name ? name : "Source Record", active, paused};
    refresh();
}
static void remove_impl(void *context)
{
    entries.erase(context);
    refresh();
}
}

extern "C" void source_record_status_init(void)
{
    if (dock || !qApp || !obs_frontend_get_main_window()) return;
    auto *mainWindow = static_cast<QMainWindow *>(obs_frontend_get_main_window());
    auto *dockWidget = new QDockWidget(QStringLiteral("Source Record Status"), mainWindow);
    dockWidget->setObjectName(QStringLiteral("SourceRecordStatusDock"));
    dockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    auto *widget = new QWidget(dockWidget);
    auto *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(12, 8, 12, 8);
    auto *label = new QLabel(QStringLiteral("●  IDLE"), widget);
    label->setAlignment(Qt::AlignCenter);
    label->setMinimumWidth(170);
    label->setStyleSheet(QStringLiteral("color:#888888;font-size:16px;font-weight:600;"));
    layout->addWidget(label);
    dockWidget->setWidget(widget);
    // OBS 29-compatible dock API. The newer add_dock_by_id API did not exist until OBS 30.
    obs_frontend_add_dock(dockWidget);
    dock = dockWidget;
    statusLabel = label;
}

extern "C" void source_record_status_shutdown(void)
{
    entries.clear();
    if (dock) {
        dock->setParent(nullptr);
        dock->deleteLater();
        dock = nullptr;
        statusLabel = nullptr;
    }
}

extern "C" void source_record_status_update(void *context, const char *name, bool active, bool paused)
{
    if (!qApp) return;
    const std::string nameCopy = name ? name : "Source Record";
    QMetaObject::invokeMethod(qApp, [context, nameCopy, active, paused] {
        if (statusLabel) update_impl(context, nameCopy.c_str(), active, paused);
    }, Qt::QueuedConnection);
}

extern "C" void source_record_status_remove(void *context)
{
    if (!qApp) return;
    QMetaObject::invokeMethod(qApp, [context] { remove_impl(context); }, Qt::QueuedConnection);
}
