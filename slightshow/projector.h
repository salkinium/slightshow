#ifndef PROJECTOR_H
#define PROJECTOR_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QSerialPort>
#include <QLoggingCategory>
#include <QPropertyAnimation>
#include <QTime>

Q_DECLARE_LOGGING_CATEGORY(slightshowProjector)

namespace slightshow
{

class Projector : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
public:
    enum class Direction
    {
        Forward,
        Backward
    };

public:
    explicit Projector(quint8 address, QObject *parent = nullptr);
    ~Projector();

    void
    setPort(QSerialPort *port);

    inline qreal
    brightness() const
    { return m_brightness; }

    inline quint8
    address() const
    { return m_address; }

    bool
    isSliding() const
    { return m_is_sliding; }

    bool
    isFading() const
    { return m_animation->state() == QAbstractAnimation::Running; }

signals:
    void
    fadeFinished();

    void
    brightnessChanged(qreal brightness, qreal scalar);

    void
    slideFinished(Direction direction);

public slots:
    void
    fade(qreal brightness, qreal time);

    void
    stop();

    void
    setBrightness(qreal brightness);

    void
    setBrightnessScalar(qreal brightness);

    void
    slide(Direction direction);

protected:
    void
    sendCommand(quint8 command, int parameter);

    void
    sendCommand(quint8 command, bool power);

    void
    sendCommand(quint8 command);

    void
    sendPacket(quint8 mode, quint8 command, quint8 parameter);

private:
    const quint8 m_address;
    QPropertyAnimation *const m_animation;

    QSerialPort *m_serial = nullptr;
    qreal m_brightness = 0;
    qreal m_brightness_scalar = 1.0;

    bool m_is_sliding = false;
};

} // namespace slightshow

#endif // PROJECTOR_H
