#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

namespace dataslinger
{
namespace ui
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

}
}
