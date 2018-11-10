#pragma once

#include <memory>

#include <QMainWindow>

#include <boost/signals2/signal.hpp>

namespace Ui {
class LauncherWindow;
}

namespace dataslinger
{
namespace app
{
class LauncherAppSignals;
}
}

namespace dataslinger
{
namespace ui
{

/// The launcher widget UI
/// Provides a widget for opening demo apps
class LauncherWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LauncherWindow(QWidget* parent = nullptr);
    ~LauncherWindow();

    /// Connects the UI to the backend app signals
    void connectToApplication(dataslinger::app::LauncherAppSignals& s);

private:
    class LauncherWindowImpl;
    std::unique_ptr<LauncherWindowImpl> d;
};

}
}
