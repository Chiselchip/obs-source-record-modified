#include "source-record-dock.h"
#include <obs-frontend-api.h>
#include <QDockWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

static QLabel* g_statusLabel = nullptr;

extern "C" void source_record_dock_init(void) {
    if (g_statusLabel) return; // Prevent double initialization

    QDockWidget* dock = new QDockWidget("Source Record Status");
    dock->setObjectName("sourceRecordStatusDock");

    QWidget* container = new QWidget(dock);
    QVBoxLayout* layout = new QVBoxLayout(container);

    g_statusLabel = new QLabel("STATUS: IDLE");
    g_statusLabel->setAlignment(Qt::AlignCenter);
    g_statusLabel->setStyleSheet("font-family: 'Montserrat', sans-serif; font-size: 18px; font-weight: bold; color: #FFFFFF;");

    layout->addWidget(g_statusLabel);
    container->setLayout(layout);
    dock->setWidget(container);

    // Binds the dock natively into the OBS View -> Docks menu
    obs_frontend_add_dock(dock); 
}

extern "C" void source_record_dock_set_paused(bool is_paused) {
    if (!g_statusLabel) return;

    if (is_paused) {
        g_statusLabel->setText("STATUS: PAUSED");
        g_statusLabel->setStyleSheet("font-family: 'Montserrat', sans-serif; font-size: 18px; font-weight: bold; color: #ff7900;");
    } else {
        g_statusLabel->setText("STATUS: RECORDING");
        g_statusLabel->setStyleSheet("font-family: 'Montserrat', sans-serif; font-size: 18px; font-weight: bold; color: #01f5fd;");
    }
}
