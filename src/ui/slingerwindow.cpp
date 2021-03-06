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

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QPlainTextEdit>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVariant>
#include <QUrl>

#include "dataslinger/slinger.h"

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

        q->setAcceptDrops(true);

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

        m_appConnections.emplace_back(s.signal_onSlingerSentData.connect([this](const dataslinger::message::Message& message) {
            ++m_sendCount;

            const std::size_t sizeBytes = message.size();
            const double sizeMBytes = static_cast<double>(sizeBytes) / 1000000;

            ui->slingingLog->appendPlainText(QString::number(m_sendCount).append(": " )
                                             .append("Sent message with size: ").append(QString::number(sizeBytes))
                                             .append(" bytes (").append(QString::number(sizeMBytes, 'G', 2)).append("MB)"));
        }));
        m_appConnections.emplace_back(s.signal_onSlingerReceivedData.connect([this](const dataslinger::message::Message& message) {
            ++m_receiveCount;

            const std::size_t sizeBytes = message.size();
            const double sizeMBytes = static_cast<double>(sizeBytes) / 1000000;

            ui->slingingLog->appendPlainText(
                        QString::number(m_receiveCount).append(": ")
                        .append("Received message with size: ").append(QString::number(sizeBytes))
                        .append(" bytes (").append(QString::number(sizeMBytes, 'G', 2)).append("MB)"));
        }));

        m_appConnections.emplace_back(s.signal_onEvent.connect([this](const dataslinger::event::Event& e) {
            ui->eventLog->appendPlainText(e.what().c_str());
        }));

        m_slingerPollingTimer.start(150);

        connect(&m_slingerPollingTimer, &QTimer::timeout, [&s]() {
            s.signal_onPollSlingersRequest();
        });

        connect(ui->slingerLaunchButton, &QPushButton::clicked, [this, &s]() {
            ui->slingerUiLog->appendPlainText("Clicked slinger launch button...");

            const auto backend = static_cast<dataslinger::connection::PreferredBackend>(ui->backendSelectionBox->currentData().toInt());

            const auto options = makeConnectionOptionsFromUserInput(ui->slingerAddressEdit->text().toStdString(), backend);
            if(options == std::nullopt) {
                QMessageBox::critical(q, "Validation Error", "Failed to validate data slinger connection info.");
                return;
            }

            s.signal_onSlingerSetupRequest(*options);
        });

        connect(ui->clearSlingersButton, &QPushButton::clicked, [this, &s]() {
            ui->slingerUiLog->appendPlainText("Clicked clear slingers button...");
            s.signal_onClearSlingersRequest();
        });

        connect(ui->commandLineEdit, &dataslinger::ui::CommandLineEdit::signal_commandSubmitted, [&s](const std::string& command) {
            s.signal_onCommand(command);
        });
    }

    void dragEnterEvent(QDragEnterEvent* event)
    {
        event->acceptProposedAction();
    }

    void dropEvent(QDropEvent* event)
    {
        const auto urls = event->mimeData()->urls();
        if(urls.empty()) {
            return;
        }
        event->acceptProposedAction();

        q->signal_onDropEvent(urls);
    }

private:
    SlingerWindow* q;
    std::unique_ptr<Ui::SlingerWindow> ui;

    QTimer m_slingerPollingTimer;

    std::vector<boost::signals2::scoped_connection> m_appConnections; ///< Boost signals2 connections between the UI and the backend application

    std::uint64_t m_receiveCount{0}; ///< Number of received messages so far by slingers on this widget
    std::uint64_t m_sendCount{0}; ///< Number of attempted sends so far by slingers on this widget
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

void SlingerWindow::dragEnterEvent(QDragEnterEvent* event)
{
    d->dragEnterEvent(event);
}

void SlingerWindow::dropEvent(QDropEvent* event)
{
    d->dropEvent(event);
}

}
}
