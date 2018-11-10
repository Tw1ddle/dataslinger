#include <QApplication>

#include "app/launcherapp.h"
#include "ui/launcherwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    dataslinger::app::LauncherApp app;
    dataslinger::ui::LauncherWindow ui;

    // Connect the user interface to the demo launcher implementation
    ui.connectToApplication(app.getSignals());

    // Show the UI
    ui.show();

    return a.exec();
}
