#include "app/launcherapp.h"

#include <memory>
#include <vector>

#include "dataslinger/connection/connectionoptions.h"

#include "app/launcherappsignals.h"
#include "app/slingerapp.h"
#include "ui/slingerwindow.h"

namespace dataslinger
{
namespace app
{

class LauncherApp::LauncherAppImpl
{
public:
    LauncherAppImpl(LauncherApp* pQ) : q{pQ}
    {
        m_signals.signal_onOpenSlingerRequest.connect([]{
            // TODO memory leaks
            dataslinger::ui::SlingerWindow* window = new dataslinger::ui::SlingerWindow();
            dataslinger::app::SlingerApp* app = new dataslinger::app::SlingerApp();

            window->connectToApplication(app->getSignals());
            window->show();
        });

        m_signals.signal_onOpenSlingerWithOptionsRequest.connect([](const dataslinger::connection::ConnectionOptions& options){
            // TODO memory leaks
            // TODO pass in options
            dataslinger::ui::SlingerWindow* window = new dataslinger::ui::SlingerWindow();
            dataslinger::app::SlingerApp* app = new dataslinger::app::SlingerApp();

            window->connectToApplication(app->getSignals());
            window->show();
        });
    }

    ~LauncherAppImpl()
    {
    }

    LauncherAppImpl(const LauncherAppImpl&) = delete;
    LauncherAppImpl& operator=(const LauncherAppImpl&) = delete;

    LauncherAppSignals& getSignals()
    {
        return m_signals;
    }

private:
    dataslinger::app::LauncherApp* q;
    dataslinger::app::LauncherAppSignals m_signals;
};

LauncherApp::LauncherApp() : d{std::make_unique<LauncherApp::LauncherAppImpl>(this)}
{
}

LauncherApp::~LauncherApp()
{
}

LauncherAppSignals& LauncherApp::getSignals()
{
    return d->getSignals();
}

}
}
