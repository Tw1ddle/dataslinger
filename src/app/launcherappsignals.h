#pragma once

#include <boost/signals2/signal.hpp>

#include "dataslinger/connection/connectionoptions.h"

namespace dataslinger
{
namespace app
{

class LauncherAppSignals
{
public:
    LauncherAppSignals() = default;
    ~LauncherAppSignals() = default;
    LauncherAppSignals(const LauncherAppSignals&) = delete;
    LauncherAppSignals& operator=(const LauncherAppSignals&) = delete;

    boost::signals2::signal<void()> signal_onOpenSlingerRequest;
    boost::signals2::signal<void(const dataslinger::connection::ConnectionOptions&)> signal_onOpenSlingerWithOptionsRequest;
};

}
}
