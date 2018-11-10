#pragma once

#include <memory>

#include <boost/signals2/signal.hpp>

#include "dataslinger/connection/connectionoptions.h"

namespace dataslinger
{
namespace app
{
class SlingerAppSignals;
}
}

namespace dataslinger
{
namespace app
{

/// The slinger app class
/// Implements the backend functionality for interacting with a slinger
class SlingerApp
{
public:
    SlingerApp();
    ~SlingerApp();
    SlingerApp(const SlingerApp&) = delete;
    SlingerApp& operator=(const SlingerApp&) = delete;

    /// Returns a reference to the signals emitted by the slinger app
    SlingerAppSignals& getSignals();

    /// Adds and starts a slinger given the given options
    void addSlinger(const dataslinger::connection::ConnectionOptions& options);

private:
    class SlingerAppImpl;
    std::unique_ptr<SlingerAppImpl> d;
};

}
}
