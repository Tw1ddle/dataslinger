#pragma once

#include <memory>

#include <boost/signals2/signal.hpp>

namespace dataslinger
{
namespace app
{
class AppSignals;
}
}

namespace dataslinger
{
namespace app
{

/// The main application class
/// Implements the backend functionality for this demo
class App
{
public:
    App();
    ~App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    /// Returns a reference to the signals emitted by the application
    AppSignals& getAppSignals();

private:
    class AppImpl;
    std::unique_ptr<AppImpl> d;
};

}
}
