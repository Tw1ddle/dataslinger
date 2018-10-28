#include "app/app.h"

#include <memory>

#include <boost/scope_exit.hpp>

#include "dataslinger/dataslingererrors.h"
#include "dataslinger/dataslinger.h"
#include "dataslinger/datareceiver.h"

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
        getAppSignals().signal_onReceiverSetupRequest.connect([this]{
            addReceiver();
        });
        getAppSignals().signal_onSlingerSetupRequest.connect([this]{
            addSlinger();
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

    void stopSlinging()
    {
        BOOST_SCOPE_EXIT(this_) {
            this_->q->signal_afterStopSlinging();
        } BOOST_SCOPE_EXIT_END

        q->signal_afterStopSlinging();

        m_slingers.clear();
    }

    void stopListening()
    {
        BOOST_SCOPE_EXIT(this_) {
            this_->q->signal_afterStopListening();
        } BOOST_SCOPE_EXIT_END

        q->signal_beforeStopListening();

        m_receivers.clear();
    }

private:

    void addSlinger()
    {
        try {
            m_slingers.emplace_back(std::move(dataslinger::DataSlinger()));
        } catch(const dataslinger::error::DataSlingerError& e) {
            m_signals.signal_onError(e);
        }

        // TODO
        //m_slinger.signal_afterSend.connect([this]() {
        //    m_signals.signal_onDataSlung();
        //});

        //m_receiver.signal_afterReceive.connect([this]() {
        //    m_signals.signal_onDataReceived();
        //});
    }

    void addReceiver()
    {
        try {
            m_receivers.emplace_back(std::move(dataslinger::DataReceiver()));
        } catch(const dataslinger::error::DataSlingerError& e) {
            m_signals.signal_onError(e);
        }
    }

    dataslinger::app::App* q;
    dataslinger::app::AppSignals m_signals;

    std::vector<dataslinger::DataSlinger> m_slingers;
    std::vector<dataslinger::DataReceiver> m_receivers;
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

void App::stopSlinging()
{
    d->stopSlinging();
}

void App::stopListening()
{
    d->stopListening();
}

}
}
