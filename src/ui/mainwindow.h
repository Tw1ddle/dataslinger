#pragma once

#include <memory>

#include <QMainWindow>

#include <boost/signals2/signal.hpp>

namespace Ui {
class MainWindow;
}

namespace dataslinger
{
namespace app
{
class AppSignals;
}
}

namespace dataslinger
{
namespace ui
{

/// The main window UI
/// Gets connected to signals from the backend/demo implementation
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    /// Signal emitted before the UI is connected to the backend
    boost::signals2::signal<void(dataslinger::app::AppSignals&)> signal_beforeConnectToApplication;
    /// Signal emitted after the UI is connected to the backend
    boost::signals2::signal<void(dataslinger::app::AppSignals&)> signal_afterConnectToApplication;
    /// Connects the UI to the backend app signals
    void connectToApplication(dataslinger::app::AppSignals& s);

private:
    class MainWindowImpl;
    std::unique_ptr<MainWindowImpl> d;
};

}
}
