#include "projector.h"
#include <QSerialPortInfo>
#include <QByteArray>
#include <QtMath>
#include <QTimer>

//Q_LOGGING_CATEGORY(slightshowProjector, "slightshow.projector", QtInfoMsg)
Q_LOGGING_CATEGORY(slightshowProjector, "slightshow.projector", QtDebugMsg)

namespace slightshow {

Projector::Projector(quint8 address, QObject *parent) :
    QObject(parent), m_address(address), m_animation(new QPropertyAnimation(this, "brightness"))
{
    connect(m_animation, &QPropertyAnimation::finished, this, &Projector::fadeFinished);
}

Projector::~Projector()
{
}

void
Projector::setPort(QSerialPort *port)
{
    m_serial = port;
}

void
Projector::fade(qreal brightness, qreal time)
{
    brightness = qFabs(brightness);
    if (brightness > m_brightness_limit)
        brightness = m_brightness_limit;

    m_animation->setDuration(time * 1000.0);
    m_animation->setStartValue(this->brightness());
    m_animation->setEndValue(brightness);
    m_animation->start();

    qCDebug(slightshowProjector) << m_address << "Fading" << brightness << "in" << time;
}

void
Projector::stop()
{
    m_animation->stop();
}

void
Projector::setBrightness(qreal brightness)
{
    brightness = qFabs(brightness);
    if (brightness > m_brightness_limit)
        brightness = m_brightness_limit;

    sendCommand(1, int(brightness * 1000.0));

    m_brightness = brightness;
    emit brightnessChanged(m_brightness);
}

void
Projector::setBrightnessLimit(qreal brightness)
{
    brightness = qFabs(brightness);
    if (brightness > 1.0) brightness = 1.0;
    m_brightness_limit = brightness;
    setBrightness(m_brightness);
}

void
Projector::slide(Direction direction)
{
    m_is_sliding = true;
    sendCommand( (direction == Direction::Forward) ? 0 : 1 );
    QTimer::singleShot(1300, [this, direction]()
    {
        emit slideFinished(direction);
        m_is_sliding = false;
    });
    qCDebug(slightshowProjector) << m_address << "Slide" << ((direction == Direction::Forward) ? "Forward" : "Backward");
}

void
Projector::sendCommand(quint8 command, int parameter)
{
    sendPacket(0, command * 16 + (parameter / 128) * 2, (parameter % 128) * 2);
}

void
Projector::sendCommand(quint8 command, bool power)
{
    sendPacket(1, command * 4 + (power ? 2 : 0), 0);
}

void
Projector::sendCommand(quint8 command)
{
    sendPacket(2, command * 4, 0);
}

void
Projector::sendPacket(quint8 mode, quint8 command, quint8 parameter)
{
    if (m_serial and m_serial->isOpen())
    {
        auto data = QByteArray(4, 0);
        data[1] = (m_address * 8) + (mode * 2) + 1;
        data[2] = command;
        data[3] = parameter;
        const auto now = QTime::currentTime();
        m_serial->write(data);
        m_serial->flush();
//        qCDebug(slightshowProjector) << "Sent packet" << now;
    }
}

} // namespace slightshow
