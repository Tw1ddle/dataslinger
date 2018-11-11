#pragma once

#include <memory>

#include <QList>
#include <QMainWindow>
#include <QUrl>

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

    /// Signal emitted when the user drops something onto the window
    boost::signals2::signal<void(const QList<QUrl>&)> signal_onDropEvent;

    /// Signal emitted before the UI is connected to the backend
    boost::signals2::signal<void(dataslinger::app::SlingerAppSignals&)> signal_beforeConnectToApplication;
    /// Signal emitted after the UI is connected to the backend
    boost::signals2::signal<void(dataslinger::app::SlingerAppSignals&)> signal_afterConnectToApplication;
    /// Connects the UI to the backend app signals
    void connectToApplication(dataslinger::app::SlingerAppSignals& s);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    class SlingerWindowImpl;
    std::unique_ptr<SlingerWindowImpl> d;
};

}
}
