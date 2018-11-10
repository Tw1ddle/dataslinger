#pragma once

#include <memory>

#include <QMainWindow>

#include <boost/signals2/signal.hpp>

namespace Ui {
class SlingerWindow;
}

namespace dataslinger
{
namespace app
{
class SlingerAppSignals;
}
}

namespace dataslinger
{
namespace ui
{

/// The main window UI
/// Gets connected to signals from the backend/demo implementation
class SlingerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SlingerWindow(QWidget* parent = nullptr);
    ~SlingerWindow();

    /// Signal emitted before the UI is connected to the backend
    boost::signals2::signal<void(dataslinger::app::SlingerAppSignals&)> signal_beforeConnectToApplication;
    /// Signal emitted after the UI is connected to the backend
    boost::signals2::signal<void(dataslinger::app::SlingerAppSignals&)> signal_afterConnectToApplication;
    /// Connects the UI to the backend app signals
    void connectToApplication(dataslinger::app::SlingerAppSignals& s);

private:
    class SlingerWindowImpl;
    std::unique_ptr<SlingerWindowImpl> d;
};

}
}
