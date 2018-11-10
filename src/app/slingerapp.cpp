#include "app/slingerapp.h"

#include <memory>
#include <vector>

#include <boost/scope_exit.hpp>

#include "dataslinger/slinger.h"
#include "dataslinger/connection/connectionoptions.h"
#include "dataslinger/message/message.h"
#include "dataslinger/event/event.h"

#include "app/slingerappsignals.h"

namespace dataslinger
{
namespace app
{

class SlingerApp::SlingerAppImpl
{
public:
    SlingerAppImpl(SlingerApp* pQ) : q{pQ}
    {
        m_signals.signal_onSlingerSetupRequest.connect([this](const dataslinger::connection::ConnectionOptions& info) {
            addSlinger(info);
        });

        m_signals.signal_onPollSlingersRequest.connect([this]() {
            pollSlingers();
        });

        m_signals.signal_onClearSlingersRequest.connect([this]() {
            clearSlingers();
        });

        m_signals.signal_onCommand.connect([this](const std::string& command) {
            std::vector<std::byte> data(4000000);

            if(command == "sendMessageToReceivers") {
                for(auto& slinger : m_slingers) {
                    slinger.send(data);
                }
            }
            if(command == "sendMessagesToReceivers") {
                for(auto& slinger : m_slingers) {
                    for(int i = 0; i < 20; i++) {
                        slinger.send(data);
                    }
                }
            }
        });
    }

    ~SlingerAppImpl()
    {
    }

    SlingerAppImpl(const SlingerAppImpl&) = delete;
    SlingerAppImpl& operator=(const SlingerAppImpl&) = delete;

    SlingerAppSignals& getSignals()
    {
        return m_signals;
    }

private:
    void addSlinger(const dataslinger::connection::ConnectionOptions& info)
    {
        const auto onReceive = [this](const dataslinger::message::Message& message) {
            m_signals.signal_onSlingerReceivedData(message);
        };
        const auto onEvent = [this](const dataslinger::event::Event& event) {
            m_signals.signal_onEvent(event);
        };
        m_slingers.emplace_back(dataslinger::makeDataSlinger(onReceive, onEvent, info));
    }

    void clearSlingers()
    {
        m_slingers.clear();
    }

    void pollSlingers()
    {
        for(auto& slinger : m_slingers) {
            slinger.poll();
        }
    }

    dataslinger::app::SlingerApp* q;
    dataslinger::app::SlingerAppSignals m_signals;

    std::vector<dataslinger::DataSlinger> m_slingers;
};

SlingerApp::SlingerApp() : d{std::make_unique<SlingerApp::SlingerAppImpl>(this)}
{
}

SlingerApp::~SlingerApp()
{
}

SlingerAppSignals& SlingerApp::getSignals()
{
    return d->getSignals();
}

}
}
