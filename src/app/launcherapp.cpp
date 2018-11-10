#include "app/launcherapp.h"

#include <memory>
#include <optional>
#include <vector>

#include "dataslinger/connection/connectionoptions.h"

#include "app/launcherappsignals.h"
#include "app/slingerapp.h"
#include "app/slingerappsignals.h"
#include "ui/slingerwindow.h"

namespace
{

void openSlinger(std::optional<const dataslinger::connection::ConnectionOptions> options)
{
    // TODO memory leaks
    dataslinger::ui::SlingerWindow* window = new dataslinger::ui::SlingerWindow();
    dataslinger::app::SlingerApp* app = new dataslinger::app::SlingerApp();

    window->connectToApplication(app->getSignals());
    window->show();

    if(options != std::nullopt) {
        app->addSlinger(*options);
    }
}

}

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
            openSlinger(std::nullopt);
        });

        m_signals.signal_onOpenSlingerWithOptionsRequest.connect([](const dataslinger::connection::ConnectionOptions& options){
            openSlinger(options);
        });

        m_signals.signal_onOpenSlingerPairWithOptionsRequest.connect([](const dataslinger::connection::ConnectionOptions& firstOptions,
                                                                     const dataslinger::connection::ConnectionOptions& secondOptions) {
            openSlinger(firstOptions);
            openSlinger(secondOptions);
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
