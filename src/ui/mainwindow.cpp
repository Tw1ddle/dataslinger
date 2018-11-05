#include "ui/mainwindow.h"

#include "ui_mainwindow.h"

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

#include "dataslinger/connection/connectioninfo.h"
#include "dataslinger/event/event.h"
#include "dataslinger/message/message.h"
#include "dataslinger/util/heterogeneousmap.h"

#include "app/appsignals.h"

namespace
{

std::optional<dataslinger::connection::ConnectionInfo> makeReceiverConnectionInfoFromUserInput(const std::string& input)
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

    return std::make_optional<dataslinger::connection::ConnectionInfo>({{{
        { dataslinger::connection::ConnectionInfoDataKeys::WEBSOCKET_RECEIVER_HOST_STRING, host },
        { dataslinger::connection::ConnectionInfoDataKeys::WEBSOCKET_RECEIVER_PORT_UINT16, port }
    }}});
}

std::optional<dataslinger::connection::ConnectionInfo> makeSlingerConnectionInfoFromUserInput(const std::string& input)
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

    return std::make_optional<dataslinger::connection::ConnectionInfo>({{{
        { dataslinger::connection::ConnectionInfoDataKeys::WEBSOCKET_SLINGER_HOST_STRING, host },
        { dataslinger::connection::ConnectionInfoDataKeys::WEBSOCKET_SLINGER_PORT_UINT16, port }
    }}});
}

}

namespace dataslinger
{
namespace ui
{

class MainWindow::MainWindowImpl
{
public:
    MainWindowImpl(MainWindow* pQ) : q{pQ}, ui(std::make_unique<Ui::MainWindow>())
    {
        ui->setupUi(q);

        connect(ui->eventLog, &QPlainTextEdit::textChanged, [this]() {
            ui->eventLog->ensureCursorVisible();
        });

        q->signal_beforeConnectToApplication.connect([this](dataslinger::app::AppSignals&) {
            m_appConnections.clear();
        });
    }

    ~MainWindowImpl()
    {
    }

    void connectToApplication(dataslinger::app::AppSignals& s)
    {
        BOOST_SCOPE_EXIT(this_, &s) {
            this_->q->signal_afterConnectToApplication(s);
        } BOOST_SCOPE_EXIT_END

        q->signal_beforeConnectToApplication(s);

        m_appConnections.emplace_back(s.signal_onDataSlung.connect([this]() {
            ui->slingingLog->appendPlainText("Sent message");
        }));

        m_appConnections.emplace_back(s.signal_onDataReceived.connect([this](const dataslinger::message::Message& message) {
            ui->receivingLog->appendPlainText("Received message");
        }));

        m_appConnections.emplace_back(s.signal_onEvent.connect([this](const dataslinger::event::Event& e) {
            const dataslinger::event::EventSourceKind source = e.getInfo().getValue<dataslinger::event::EventSourceKind>(dataslinger::event::EventDataKeys::EVENT_SOURCE_KIND);

            switch(source) {
                case dataslinger::event::EventSourceKind::RECEIVER:
                    ui->receivingLog->appendPlainText(e.what().c_str());
                    ui->receivingLog->appendPlainText("\n");
                    break;
                case dataslinger::event::EventSourceKind::SLINGER:
                    ui->slingingLog->appendPlainText(e.what().c_str());
                    ui->slingingLog->appendPlainText("\n");
                    break;
                default:
                    assert(0 && "Received event from unknown source");
                    ui->eventLog->appendPlainText(e.what().c_str());
                    ui->eventLog->appendPlainText("\n");
                    break;
            }
        }));

        m_receiverPollingTimer.start(150);
        m_slingerPollingTimer.start(150);

        connect(&m_receiverPollingTimer, &QTimer::timeout, [&s]() {
            s.signal_onPollReceiversRequest();
        });

        connect(&m_slingerPollingTimer, &QTimer::timeout, [&s]() {
            s.signal_onPollSlingersRequest();
        });

        connect(ui->slingerLaunchButton, &QPushButton::clicked, [this, &s]() {
            ui->slingerConnectionLog->appendPlainText("Clicked slinger launch button...\n");

            const auto connectionInfo = makeSlingerConnectionInfoFromUserInput(ui->slingerAddressEdit->text().toStdString());

            if(connectionInfo == std::nullopt) {
                QMessageBox::critical(q, "Validation Error", "Failed to validate data slinger connection info.");
                return;
            }

            s.signal_onSlingerSetupRequest(*connectionInfo);
        });

        connect(ui->clientConnectButton, &QPushButton::clicked, [this, &s]() {
            ui->clientLog->appendPlainText("Clicked client connect button...\n");

            const auto connectionInfo = makeReceiverConnectionInfoFromUserInput(ui->clientConnectionEdit->text().toStdString());

            if(connectionInfo == std::nullopt) {
                QMessageBox::critical(q, "Validation Error", "Failed to validate data receiver connection info.");
                return;
            }

            s.signal_onReceiverSetupRequest(*connectionInfo);
        });

        connect(ui->clearSlingersButton, &QPushButton::clicked, [this, &s]() {
            ui->slingerConnectionLog->appendPlainText("Clicked clear slingers button...\n");
            s.signal_onClearSlingersRequest();
        });

        connect(ui->clearReceiversButton, &QPushButton::clicked, [this, &s]() {
            ui->clientLog->appendPlainText("Clicked clear receivers button...\n");
            s.signal_onClearReceiversRequest();
        });
    }

private:
    MainWindow* q;
    std::unique_ptr<Ui::MainWindow> ui;

    QTimer m_receiverPollingTimer;
    QTimer m_slingerPollingTimer;

    std::vector<boost::signals2::scoped_connection> m_appConnections; ///< Boost signals2 connections between the UI and the backend application
};

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), d{std::make_unique<MainWindow::MainWindowImpl>(this)}
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::connectToApplication(dataslinger::app::AppSignals& s)
{
    d->connectToApplication(s);
}

}
}
