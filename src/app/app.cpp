#include "app/app.h"

#include <memory>
#include <vector>

#include <boost/scope_exit.hpp>

#include "dataslinger/slinger.h"
#include "dataslinger/connection/connectioninfo.h"
#include "dataslinger/message/message.h"
#include "dataslinger/event/event.h"

#include "app/appsignals.h"

namespace dataslinger
{
namespace app
{

class App::AppImpl
{
public:
    AppImpl(App* pQ) : q{pQ}
    {
        m_signals.signal_onReceiverSetupRequest.connect([this](const dataslinger::connection::ConnectionInfo& info) {
            addReceiver(info);
        });
        m_signals.signal_onSlingerSetupRequest.connect([this](const dataslinger::connection::ConnectionInfo& info) {
            addSlinger(info);
        });

        m_signals.signal_onPollReceiversRequest.connect([this]() {
            pollReceivers();
        });
        m_signals.signal_onPollSlingersRequest.connect([this]() {
            pollSlingers();
        });

        m_signals.signal_onClearReceiversRequest.connect([this]() {
            clearReceivers();
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
            } else if(command =="sendMessageToSlingers") {
                for(auto& receiver : m_receivers) {
                    receiver.send(data);
                }
            }

            if(command == "sendMessagesToReceivers") {
                for(auto& slinger : m_slingers) {
                    for(int i = 0; i < 20; i++) {
                        slinger.send(data);
                    }
                }
            } else if(command =="sendMessagesToSlingers") {
                for(auto& receiver : m_receivers) {
                    for(int i = 0; i < 20; i++) {
                        receiver.send(data);
                    }
                }
            }
        });
    }

    ~AppImpl()
    {
    }

    AppImpl(const AppImpl&) = delete;
    AppImpl& operator=(const AppImpl&) = delete;

    AppSignals& getAppSignals()
    {
        return m_signals;
    }

private:
    void addSlinger(const dataslinger::connection::ConnectionInfo& info)
    {
        const auto onReceive = [this](const dataslinger::message::Message& message) {
            m_signals.signal_onSlingerReceivedData(message);
        };
        const auto onEvent = [this](const dataslinger::event::Event& event) {
            m_signals.signal_onEvent(event);
        };
        m_slingers.emplace_back(dataslinger::makeDataSlinger(onReceive, onEvent, info));
    }

    void addReceiver(const dataslinger::connection::ConnectionInfo& info)
    {
        const auto onReceive = [this](const dataslinger::message::Message& message) {
            m_signals.signal_onReceiverReceivedData(message);
        };
        const auto onEvent = [this](const dataslinger::event::Event& event) {
            m_signals.signal_onEvent(event);
        };
        m_receivers.emplace_back(dataslinger::makeDataSlinger(onReceive, onEvent, info));
    }

    void clearSlingers()
    {
        BOOST_SCOPE_EXIT(this_) {
            this_->m_signals.signal_afterClearSlingers();
        } BOOST_SCOPE_EXIT_END

        m_signals.signal_beforeClearSlingers();

        m_slingers.clear();
    }

    void clearReceivers()
    {
        BOOST_SCOPE_EXIT(this_) {
            this_->m_signals.signal_afterClearReceivers();
        } BOOST_SCOPE_EXIT_END

        m_signals.signal_beforeClearReceivers();

        m_receivers.clear();
    }

    void pollSlingers()
    {
        for(auto& slinger : m_slingers) {
            slinger.poll();
        }
    }

    void pollReceivers()
    {
        for(auto& receiver : m_receivers) {
            receiver.poll();
        }
    }

    dataslinger::app::App* q;
    dataslinger::app::AppSignals m_signals;

    std::vector<dataslinger::DataSlinger> m_slingers;
    std::vector<dataslinger::DataSlinger> m_receivers;
};

App::App() : d{std::make_unique<App::AppImpl>(this)}
{
}

App::~App()
{
}

AppSignals& App::getAppSignals()
{
    return d->getAppSignals();
}

}
}
