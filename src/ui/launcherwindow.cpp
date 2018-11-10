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
        //m_appConnections.emplace_back(s.signal_onReceiverReceivedData.connect([this](const dataslinger::message::Message& message) {
        //    ui->receivingLog->appendPlainText(QString("Received message with size: ").append(QString::number(message.size())));
        //}));

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
            s.signal_onOpenSlingerWithOptionsRequest({{{}}}); // TODO options
        }));
        grid->addWidget(makeButton("Open WebSocket Client (127.0.0.1:8081)", [&s]() {
            s.signal_onOpenSlingerWithOptionsRequest({{{}}}); // TODO options
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
