#include "source-record-dock.h"

#include <obs-frontend-api.h>

#include <QDockWidget>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <QString>

class SourceRecordStatusWidget final : public QWidget {
public:
    SourceRecordStatusWidget()
    {
        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setSpacing(6);

        title_ = new QLabel(tr("Source Record"), this);
        title_->setStyleSheet("font-weight: 600; font-size: 13px;");

        status_ = new QLabel(this);
        status_->setTextFormat(Qt::RichText);
        status_->setWordWrap(true);
        status_->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        layout->addWidget(title_);
        layout->addWidget(status_);
        layout->addStretch();

        auto *timer = new QTimer(this);
        timer->setInterval(250);
        connect(timer, &QTimer::timeout, this, &SourceRecordStatusWidget::refresh);
        timer->start();

        refresh();
    }

private:
    void refresh()
    {
        char buffer[4096] = {};
        source_record_get_status(buffer, sizeof(buffer));

        QString text = QString::fromUtf8(buffer);
        status_->setText(text);

        // Keep the dock compact while still making PAUSED visually obvious.
        const bool paused = text.contains("PAUSED", Qt::CaseInsensitive);
        if (paused) {
            status_->setStyleSheet("font-size: 12px; color: #e6b800;");
        } else if (text.contains("RECORDING", Qt::CaseInsensitive)) {
            status_->setStyleSheet("font-size: 12px;");
        } else {
            status_->setStyleSheet("font-size: 12px; color: palette(mid);");
        }
    }

    QLabel *title_ = nullptr;
    QLabel *status_ = nullptr;
};

static QDockWidget *source_record_status_dock = nullptr;

void source_record_dock_create()
{
    if (source_record_status_dock)
        return;

    auto *widget = new SourceRecordStatusWidget();

    source_record_status_dock = new QDockWidget(QObject::tr("Source Record Status"),
                                                static_cast<QWidget *>(obs_frontend_get_main_window()));
    source_record_status_dock->setObjectName("SourceRecordStatusDock");
    source_record_status_dock->setWidget(widget);
    source_record_status_dock->setMinimumSize(220, 100);
    source_record_status_dock->resize(260, 140);

    obs_frontend_add_custom_qdock("source_record_status", source_record_status_dock);
}

void source_record_dock_destroy()
{
    if (!source_record_status_dock)
        return;

    obs_frontend_remove_dock("source_record_status");
    source_record_status_dock = nullptr;
}
