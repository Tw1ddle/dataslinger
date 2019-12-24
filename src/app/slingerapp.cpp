#include "app/slingerapp.h"

#include <chrono>
#include <memory>
#include <random>
#include <vector>

#include "dataslinger/slinger.h"

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
            std::vector<std::uint8_t> data(4000000);

            if(command == "sendMessage") {
                send(data);
            }
            if(command == "sendMessages") {
                for(int i = 0; i < 25; i++) {
                    send(data);
                }
            }
            if(command == "sendRandomMessages") {
                std::default_random_engine generator;
                generator.seed(static_cast<std::uint32_t>(std::chrono::system_clock::now().time_since_epoch().count()));
                std::uniform_int_distribution<std::size_t> distribution(1, 100000);
                for(int i = 0; i < 25; i++) {
                    send(std::vector<std::uint8_t>(distribution(generator)));
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

    void send(const dataslinger::message::Message& data)
    {
        for(auto& slinger : m_slingers) {
            slinger.send(data);
        }
        m_signals.signal_onSlingerSentData(data);
    }

private:
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

void SlingerApp::addSlinger(const dataslinger::connection::ConnectionOptions& options)
{
    return d->addSlinger(options);
}

void SlingerApp::send(const dataslinger::message::Message& data)
{
    d->send(data);
}

}
}
