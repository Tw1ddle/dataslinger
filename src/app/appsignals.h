#pragma once

#include <boost/signals2/signal.hpp>

#include "dataslinger/connection/connectioninfo.h"
#include "dataslinger/event/event.h"
#include "dataslinger/message/message.h"

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

    boost::signals2::signal<void(dataslinger::connection::ConnectionInfo)> signal_onReceiverSetupRequest;
    boost::signals2::signal<void(dataslinger::connection::ConnectionInfo)> signal_onSlingerSetupRequest;

    boost::signals2::signal<void()> signal_onPollReceiversRequest;
    boost::signals2::signal<void()> signal_onPollSlingersRequest;

    boost::signals2::signal<void(const dataslinger::message::Message&)> signal_onDataReceived;
    boost::signals2::signal<void()> signal_onDataSlung;

    boost::signals2::signal<void(dataslinger::event::Event)> signal_onEvent;

    boost::signals2::signal<void()> signal_onClearReceiversRequest;
    boost::signals2::signal<void()> signal_onClearSlingersRequest;

    boost::signals2::signal<void()> signal_beforeClearSlingers;
    boost::signals2::signal<void()> signal_afterClearSlingers;

    boost::signals2::signal<void()> signal_beforeClearReceivers;
    boost::signals2::signal<void()> signal_afterClearReceivers;

    boost::signals2::signal<void(const std::string&)> signal_onCommand;
};

}
}
