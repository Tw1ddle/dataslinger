#include "ui/slingerwindow.h"

#include "ui_slingerwindow.h"

#include <cassert>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/scope_exit.hpp>

#include <QMessageBox>
#include <QPlainTextEdit>
#include <QString>
#include <QTimer>
#include <QVariant>

#include "dataslinger/connection/connectionoptions.h"
#include "dataslinger/event/event.h"
#include "dataslinger/message/message.h"

#include "app/slingerappsignals.h"

namespace
{

std::optional<dataslinger::connection::ConnectionOptions> makeConnectionOptionsFromUserInput(const std::string& input, const dataslinger::connection::PreferredBackend backend)
{
    std::vector<std::string> parts;
    boost::algorithm::split(parts, input, boost::is_any_of(":"));

    const std::string host = parts.size() >= 1 ? parts[0] : "";
    const std::string portStr = parts.size() >= 2 ? parts[1] : "";

    if(host.empty() || portStr.empty()) {
        return std::nullopt;
    }

    std::uint16_t port = 0;
    try {
        port = std::clamp(std::stoi(portStr), 0, static_cast<std::int32_t>(std::numeric_limits<std::uint16_t>::max()));
    } catch(...) {
        assert(0 && "Failed to convert port string to integer");
        return std::nullopt;
    }

    return std::make_optional<dataslinger::connection::ConnectionOptions>({{{
        { dataslinger::connection::ConnectionOption::PREFERRED_BACKEND, backend },
        { dataslinger::connection::ConnectionOption::WEBSOCKET_HOST_STRING, host },
        { dataslinger::connection::ConnectionOption::WEBSOCKET_PORT_UINT16, port }
    }}});
}

}

namespace dataslinger
{
namespace ui
{

class SlingerWindow::SlingerWindowImpl
{
public:
    SlingerWindowImpl(SlingerWindow* pQ) : q{pQ}, ui(std::make_unique<Ui::SlingerWindow>())
    {
        ui->setupUi(q);

        connect(ui->eventLog, &QPlainTextEdit::textChanged, [this]() {
            ui->eventLog->ensureCursorVisible();
        });

        q->signal_beforeConnectToApplication.connect([this](dataslinger::app::SlingerAppSignals&) {
            m_appConnections.clear();
        });

        ui->backendSelectionBox->addItem("Pipe", QVariant(static_cast<int>(dataslinger::connection::PreferredBackend::PIPE)));
        ui->backendSelectionBox->addItem("Shared Vector", QVariant(static_cast<int>(dataslinger::connection::PreferredBackend::SHARED_VECTOR)));
        ui->backendSelectionBox->addItem("WebSocket Client", QVariant(static_cast<int>(dataslinger::connection::PreferredBackend::WEBSOCKET_CLIENT)));
        ui->backendSelectionBox->addItem("WebSocket Server", QVariant(static_cast<int>(dataslinger::connection::PreferredBackend::WEBSOCKET_SERVER)));

        ui->backendSelectionBox->setCurrentIndex(static_cast<int>(dataslinger::connection::PreferredBackend::WEBSOCKET_CLIENT));
    }

    ~SlingerWindowImpl()
    {
    }

    void connectToApplication(dataslinger::app::SlingerAppSignals& s)
    {
        BOOST_SCOPE_EXIT(this_, &s) {
            this_->q->signal_afterConnectToApplication(s);
        } BOOST_SCOPE_EXIT_END

        q->signal_beforeConnectToApplication(s);

        m_appConnections.emplace_back(s.signal_onSlingerSentData.connect([this]() {
            ui->slingingLog->appendPlainText("Slinger sent message");
        }));

        m_appConnections.emplace_back(s.signal_onSlingerReceivedData.connect([this](const dataslinger::message::Message& message) {
            ui->slingingLog->appendPlainText(QString("Received message with size: ").append(QString::number(message.size())));
        }));

        m_appConnections.emplace_back(s.signal_onEvent.connect([this](const dataslinger::event::Event& e) {
            const dataslinger::event::EventSourceKind source = e.getInfo().getValue<dataslinger::event::EventSourceKind>(dataslinger::event::EventDataKeys::EVENT_SOURCE_KIND);

            ui->slingingLog->appendPlainText(e.what().c_str());
            ui->slingingLog->appendPlainText("\n");
        }));

        m_slingerPollingTimer.start(150);

        connect(&m_slingerPollingTimer, &QTimer::timeout, [&s]() {
            s.signal_onPollSlingersRequest();
        });

        connect(ui->slingerLaunchButton, &QPushButton::clicked, [this, &s]() {
            ui->slingerConnectionLog->appendPlainText("Clicked slinger launch button...\n");

            const auto backend = static_cast<dataslinger::connection::PreferredBackend>(ui->backendSelectionBox->currentData().toInt());

            const auto options = makeConnectionOptionsFromUserInput(ui->slingerAddressEdit->text().toStdString(), backend);
            if(options == std::nullopt) {
                QMessageBox::critical(q, "Validation Error", "Failed to validate data slinger connection info.");
                return;
            }

            s.signal_onSlingerSetupRequest(*options);
        });

        connect(ui->clearSlingersButton, &QPushButton::clicked, [this, &s]() {
            ui->slingerConnectionLog->appendPlainText("Clicked clear slingers button...\n");
            s.signal_onClearSlingersRequest();
        });

        connect(ui->commandLineEdit, &dataslinger::ui::CommandLineEdit::signal_commandSubmitted, [&s](const std::string& command) {
            s.signal_onCommand(command);
        });
    }

private:
    SlingerWindow* q;
    std::unique_ptr<Ui::SlingerWindow> ui;

    QTimer m_slingerPollingTimer;

    std::vector<boost::signals2::scoped_connection> m_appConnections; ///< Boost signals2 connections between the UI and the backend application
};

SlingerWindow::SlingerWindow(QWidget* parent) : QMainWindow(parent), d{std::make_unique<SlingerWindow::SlingerWindowImpl>(this)}
{
}

SlingerWindow::~SlingerWindow()
{
}

void SlingerWindow::connectToApplication(dataslinger::app::SlingerAppSignals& s)
{
    d->connectToApplication(s);
}

}
}
