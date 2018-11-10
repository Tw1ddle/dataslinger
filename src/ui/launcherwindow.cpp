#include "ui/launcherwindow.h"

#include "ui_launcherwindow.h"

#include <functional>
#include <memory>

#include <QGridLayout>
#include <QPushButton>
#include <QString>

#include "app/launcherappsignals.h"

namespace dataslinger
{
namespace ui
{

class LauncherWindow::LauncherWindowImpl
{
public:
    LauncherWindowImpl(LauncherWindow* pQ) : q{pQ}, ui(std::make_unique<Ui::LauncherWindow>())
    {
        ui->setupUi(q);
    }

    ~LauncherWindowImpl()
    {
    }

    void connectToApplication(dataslinger::app::LauncherAppSignals& s)
    {
        auto grid = ui->verticalLayout_3->findChild<QGridLayout*>();

        auto makeButton = [this](const QString& label, const std::function<void()>& onClick) {
            auto button = new QPushButton(label, nullptr);
            connect(button, &QPushButton::clicked, [onClick]() {
                onClick();
            });
            return button;
        };

        grid->addWidget(makeButton("Open Empty Slinger", [&s]() {
            s.signal_onOpenSlingerRequest();
        }));
        grid->addWidget(makeButton("Open WebSocket Server (127.0.0.1:8081)", [&s]() {
            s.signal_onOpenSlingerWithOptionsRequest(
            {{{
                 { dataslinger::connection::ConnectionOption::PREFERRED_BACKEND, dataslinger::connection::PreferredBackend::WEBSOCKET_SERVER },
                 { dataslinger::connection::ConnectionOption::WEBSOCKET_HOST_STRING, std::string("127.0.0.1") },
                 { dataslinger::connection::ConnectionOption::WEBSOCKET_PORT_UINT16, static_cast<std::uint16_t>(8001) }
            }}});
        }));
        grid->addWidget(makeButton("Open WebSocket Client (127.0.0.1:8081)", [&s]() {
            s.signal_onOpenSlingerWithOptionsRequest(
            {{{
                 { dataslinger::connection::ConnectionOption::PREFERRED_BACKEND, dataslinger::connection::PreferredBackend::WEBSOCKET_CLIENT },
                 { dataslinger::connection::ConnectionOption::WEBSOCKET_HOST_STRING, std::string("127.0.0.1") },
                 { dataslinger::connection::ConnectionOption::WEBSOCKET_PORT_UINT16, static_cast<std::uint16_t>(8001) }
            }}});
        }));
        grid->addWidget(makeButton("Open WebSocket Server/Client Pair (127.0.0.1:8081)", [&s]() {
            s.signal_onOpenSlingerPairWithOptionsRequest(
            {{{
                 { dataslinger::connection::ConnectionOption::PREFERRED_BACKEND, dataslinger::connection::PreferredBackend::WEBSOCKET_SERVER },
                 { dataslinger::connection::ConnectionOption::WEBSOCKET_HOST_STRING, std::string("127.0.0.1") },
                 { dataslinger::connection::ConnectionOption::WEBSOCKET_PORT_UINT16, static_cast<std::uint16_t>(8001) }
            }}},
            {{{
                 { dataslinger::connection::ConnectionOption::PREFERRED_BACKEND, dataslinger::connection::PreferredBackend::WEBSOCKET_CLIENT },
                 { dataslinger::connection::ConnectionOption::WEBSOCKET_HOST_STRING, std::string("127.0.0.1") },
                 { dataslinger::connection::ConnectionOption::WEBSOCKET_PORT_UINT16, static_cast<std::uint16_t>(8001) }
            }}});
        }));
        grid->addWidget(makeButton("Open Shared Memory Writer/Reader Pair", [&s]() {
            s.signal_onOpenSlingerPairWithOptionsRequest(
            {{{
                 { dataslinger::connection::ConnectionOption::PREFERRED_BACKEND, dataslinger::connection::PreferredBackend::SHARED_VECTOR }
            }}},
            {{{
                 { dataslinger::connection::ConnectionOption::PREFERRED_BACKEND, dataslinger::connection::PreferredBackend::SHARED_VECTOR }
            }}});
        }));
        grid->addWidget(makeButton("Open Pipe Writer/Reader Pair", [&s]() {
            s.signal_onOpenSlingerPairWithOptionsRequest(
            {{{
                 { dataslinger::connection::ConnectionOption::PREFERRED_BACKEND, dataslinger::connection::PreferredBackend::PIPE }
            }}},
            {{{
                 { dataslinger::connection::ConnectionOption::PREFERRED_BACKEND, dataslinger::connection::PreferredBackend::PIPE }
            }}});
        }));
    }

private:
    LauncherWindow* q;
    std::unique_ptr<Ui::LauncherWindow> ui;
};

LauncherWindow::LauncherWindow(QWidget* parent) : QMainWindow(parent), d{std::make_unique<LauncherWindow::LauncherWindowImpl>(this)}
{
}

LauncherWindow::~LauncherWindow()
{
}

void LauncherWindow::connectToApplication(dataslinger::app::LauncherAppSignals& s)
{
    d->connectToApplication(s);
}

}
}
