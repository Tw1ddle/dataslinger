#pragma once

#include <memory>

#include <boost/signals2/signal.hpp>

namespace dataslinger
{
namespace app
{
class LauncherAppSignals;
}
}

namespace dataslinger
{
namespace app
{

/// The main application class
/// Implements the backend functionality for this demo
class LauncherApp
{
public:
    LauncherApp();
    ~LauncherApp();
    LauncherApp(const LauncherApp&) = delete;
    LauncherApp& operator=(const LauncherApp&) = delete;

    /// Returns a reference to the signals emitted by the application
    LauncherAppSignals& getSignals();

private:
    class LauncherAppImpl;
    std::unique_ptr<LauncherAppImpl> d;
};

}
}
