#include "app/app.h"

#include <memory>

#include <boost/scope_exit.hpp>

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
        m_slinger.signal_afterSend.connect([this]() {
            m_signals.signal_onDataSlung();
        });

        m_receiver.signal_afterReceive.connect([this]() {
            m_signals.signal_onDataReceived();
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

    void startSlinging()
    {
        BOOST_SCOPE_EXIT(&q) {
            q->signal_afterStartSlinging();
        } BOOST_SCOPE_EXIT_END

        q->signal_beforeStartSlinging();

        // TODO
    }

    void stopSlinging()
    {
        BOOST_SCOPE_EXIT(&q) {
            q->signal_afterStopSlinging();
        } BOOST_SCOPE_EXIT_END

        q->signal_afterStopSlinging();

        // TODO
    }

    void startListening()
    {
        BOOST_SCOPE_EXIT(&q) {
            q->signal_afterStartListening();
        } BOOST_SCOPE_EXIT_END

        q->signal_beforeStartListening();

        // TODO
    }

    void stopListening()
    {
        BOOST_SCOPE_EXIT(&q) {
            q->signal_afterStopListening();
        } BOOST_SCOPE_EXIT_END

        q->signal_beforeStopListening();

        // TODO
    }

private:
    dataslinger::app::App* q;
    dataslinger::app::AppSignals m_signals;

    dataslinger::DataSlinger m_slinger;
    dataslinger::DataReceiver m_receiver;
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

void App::startSlinging()
{
    d->startSlinging();
}

void App::stopSlinging()
{
    d->stopSlinging();
}

void App::startListening()
{
    d->startListening();
}

void App::stopListening()
{
    d->stopListening();
}

}
}
