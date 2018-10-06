#pragma once

#include <boost/signals2/signal.hpp>

namespace dataslinger
{
namespace app
{

class AppSignals
{
public:
    AppSignals() = default;
    ~AppSignals() = default;
    AppSignals(const AppSignals&) = delete;
    AppSignals& operator=(const AppSignals&) = delete;

    boost::signals2::signal<void()> signal_onDataReceived;
    boost::signals2::signal<void()> signal_onDataSlung;
};

}
}
