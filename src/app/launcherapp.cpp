#include "app/launcherapp.h"

#include <cstdint>
#include <chrono>
#include <memory>
#include <optional>
#include <random>
#include <tuple>
#include <vector>

#include <QImage>
#include <QLabel>
#include <QList>
#include <QMessageBox>
#include <QMimeData>
#include <QUrl>

#include "dataslinger/connection/connectionoptions.h"

#include "app/launcherappsignals.h"
#include "app/slingerapp.h"
#include "app/slingerappsignals.h"
#include "ui/slingerwindow.h"

namespace
{

std::tuple<dataslinger::ui::SlingerWindow*, dataslinger::app::SlingerApp*> openSlinger(std::optional<const dataslinger::connection::ConnectionOptions> options)
{
    // TODO memory leaks, should make it live and die with the window
    dataslinger::ui::SlingerWindow* window = new dataslinger::ui::SlingerWindow();
    dataslinger::app::SlingerApp* app = new dataslinger::app::SlingerApp();

    window->connectToApplication(app->getSignals());
    window->show();

    if(options != std::nullopt) {
        app->addSlinger(*options);
    }

    return std::make_tuple(window, app);
}

void serializeInt32(std::byte* buf, std::int32_t val)
{
    std::uint32_t uval = static_cast<std::uint32_t>(val);
    buf[0] = static_cast<std::byte>(uval);
    buf[1] = static_cast<std::byte>(uval >> 8);
    buf[2] = static_cast<std::byte>(uval >> 16);
    buf[3] = static_cast<std::byte>(uval >> 24);
}

std::int32_t parseInt32(const std::byte* buf)
{
    // This prevents buf[i] from being promoted to a signed int.
    std::uint32_t u0 = static_cast<std::uint32_t>(buf[0]);
    std::uint32_t u1 = static_cast<std::uint32_t>(buf[1]);
    std::uint32_t u2 = static_cast<std::uint32_t>(buf[2]);
    std::uint32_t u3 = static_cast<std::uint32_t>(buf[3]);

    std::uint32_t uval = u0 | (u1 << 8) | (u2 << 16) | (u3 << 24);
    return static_cast<std::int32_t>(uval);
}

dataslinger::message::Message makeMessageFromImage(const QImage& image)
{
    // Stored as width, height 4-byte values, followed by byte data
    dataslinger::message::Message data;
    data.resize(8);
    const std::int32_t width = image.width();
    const std::int32_t height = image.height();
    serializeInt32(&data[0], width);
    serializeInt32(&data[4], width);

    for(std::size_t i = 0 ; i < static_cast<std::size_t>(image.sizeInBytes()); i++) {
        data.push_back(static_cast<std::byte>(*(image.bits() + i)));
    }

    return data;
}

std::optional<QImage> makeImageFromData(const std::vector<std::byte>& data)
{
    // Read out width and height, followed by byte data
    if(data.size() <= 8) {
        return std::nullopt;
    }

    const std::vector<std::byte> widthAndHeight(data.begin(), data.begin() + 8);

    const std::int32_t width = parseInt32(widthAndHeight.data());
    const std::int32_t height = parseInt32(widthAndHeight.data() + 4);

    const std::size_t expectedSize = static_cast<std::size_t>(8 + (width * height * 4));
    if(data.size() <= expectedSize) {
        return std::nullopt;
    }

    // Assumes RGBA8888 format
    QImage image(reinterpret_cast<const uchar*>(data.data() + 8), static_cast<int>(width), static_cast<int>(height), QImage::Format_RGBA8888);
    return image;
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

        m_signals.signal_onOpenSlingerImageDemoRequest.connect([](const dataslinger::connection::ConnectionOptions& options) {

            // Add handler methods for sending/receiving image data
            auto [window, app] = openSlinger(options);
            window->signal_onDropEvent.connect([app = app, window = window](const QList<QUrl>& urls) {
                for(const QUrl& url : urls) {
                    if(!url.isValid()) {
                        continue;
                    }
                    const QString filePath = url.toLocalFile();
                    QImage image(filePath);
                    if(image.isNull()) {
                        continue;
                    }
                    image = image.convertToFormat(QImage::Format_RGBA8888);
                    if(image.isNull()) {
                        continue;
                    }
                    const auto data = makeMessageFromImage(image);

                    //QLabel* imgLabel = new QLabel(window);
                    //imgLabel->setText("Server Side Image");
                    //imgLabel->setWindowFlags(Qt::Window);
                    //imgLabel->setPixmap(QPixmap::fromImage(image));
                    //imgLabel->show();

                    app->send(data);
                }
            });

            app->getSignals().signal_onSlingerReceivedData.connect([window = window](const dataslinger::message::Message& message) {
                const auto image = makeImageFromData(message);
                if(image == std::nullopt) {
                    QMessageBox::warning(nullptr, "Failed to make image", "Failed to load image, bad message data");
                    return;
                }
                if(image->isNull() || image->width() >= 4096 || image->height() >= 4096) {
                    QMessageBox::warning(nullptr, "Failed to make image", "It's null, or too small or large");
                    return;
                }

                QLabel* imgLabel = new QLabel(window);
                imgLabel->setText("Slung Image");
                imgLabel->setWindowFlags(Qt::Window);
                imgLabel->setPixmap(QPixmap::fromImage(*image));
                imgLabel->show();
            });
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
