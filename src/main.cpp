#include <QApplication>

#include "app/app.h"
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // TODO divide into multiple applications/windows, with the slinger and receiver(s)?
    dataslinger::app::App app;
    dataslinger::ui::MainWindow ui;

    // Connect the user interface to the demo app backend/implementation
    ui.connectToApplication(app.getAppSignals());

    // Show the UI
    ui.show();

    return a.exec();
}
