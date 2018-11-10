#pragma once

#include <string>

#include <boost/signals2/signal.hpp>

#include "dataslinger/connection/connectionoptions.h"
#include "dataslinger/event/event.h"
#include "dataslinger/message/message.h"

namespace dataslinger
{
namespace app
{

class SlingerAppSignals
{
public:
    SlingerAppSignals() = default;
    ~SlingerAppSignals() = default;
    SlingerAppSignals(const SlingerAppSignals&) = delete;
    SlingerAppSignals& operator=(const SlingerAppSignals&) = delete;

    boost::signals2::signal<void(dataslinger::connection::ConnectionOptions)> signal_onSlingerSetupRequest;
    boost::signals2::signal<void()> signal_onPollSlingersRequest;
    boost::signals2::signal<void()> signal_onSlingerSentData;
    boost::signals2::signal<void(const dataslinger::message::Message&)> signal_onSlingerReceivedData;
    boost::signals2::signal<void(dataslinger::event::Event)> signal_onEvent;
    boost::signals2::signal<void()> signal_onClearSlingersRequest;

    boost::signals2::signal<void(const std::string&)> signal_onCommand;
};

}
}
