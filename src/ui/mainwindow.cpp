#include "ui/mainwindow.h"

#include "ui_mainwindow.h"

namespace dataslinger
{
namespace ui
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

}
}
