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

    /// Signal emitted before the application stops slinging data
    boost::signals2::signal<void()> signal_beforeStopSlinging;
    /// Signal emitted after the application stops slinging data
    boost::signals2::signal<void()> signal_afterStopSlinging;
    /// Makes the application stop slinging data
    void stopSlinging();

    /// Signal emitted before the application stops listening for/receiving data
    boost::signals2::signal<void()> signal_beforeStopListening;
    /// Signal emitted after the application stops listening for/receiving data
    boost::signals2::signal<void()> signal_afterStopListening;
    /// Makes the application stop listening for/receiving data
    void stopListening();

private:
    class AppImpl;
    std::unique_ptr<AppImpl> d;
};

}
}
