#include "ui/mainwindow.h"

#include "ui_mainwindow.h"

#include <memory>
#include <vector>

#include <boost/scope_exit.hpp>

#include <QPlainTextEdit>

#include "dataslinger/dataslingererrors.h"

#include "app/appsignals.h"

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
            ui->slingingLog->insertPlainText("Test");
        }));

        m_appConnections.emplace_back(s.signal_onDataReceived.connect([this]() {
            ui->receivingLog->insertPlainText("Test");
        }));

        m_appConnections.emplace_back(s.signal_onError.connect([this](const dataslinger::error::DataSlingerError& e) {
            ui->errorLog->insertPlainText(e.what().c_str());
            ui->errorLog->insertPlainText("\n");
        }));

        connect(ui->slingerLaunchButton, &QPushButton::clicked, [this, &s]() {
            ui->slingerConnectionLog->insertPlainText("Clicked slinger launch button...\n");

            s.signal_onSlingerSetupRequest();
        });

        connect(ui->clientConnectButton, &QPushButton::clicked, [this, &s]() {
            ui->clientLog->insertPlainText("Clicked client connect button...\n");

            s.signal_onReceiverSetupRequest();
        });
    }

private:
    MainWindow* q;
    std::unique_ptr<Ui::MainWindow> ui;

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
