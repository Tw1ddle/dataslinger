#include <iostream>

#include <QApplication>

#include "ui/mainwindow.h"

#include "dataslinger/dataslinger.h"

int main(int argc, char *argv[])
{
    dataslinger::DataSlinger slinger;
    slinger.signal_beforeSend.connect([]() {
        std::cout << "Will send data" << std::endl;
    });

    QApplication a(argc, argv);
    dataslinger::ui::MainWindow w;
    w.show();

    return a.exec();
}
