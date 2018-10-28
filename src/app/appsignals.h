#pragma once

#include <boost/signals2/signal.hpp>

#include "dataslinger/dataslingererrors.h"

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

    boost::signals2::signal<void()> signal_onReceiverSetupRequest;
    boost::signals2::signal<void()> signal_onSlingerSetupRequest;

    boost::signals2::signal<void()> signal_onDataReceived;
    boost::signals2::signal<void()> signal_onDataSlung;

    boost::signals2::signal<void(dataslinger::error::DataSlingerError)> signal_onError;
};

}
}
