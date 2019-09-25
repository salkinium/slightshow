#include "projectormanager.h"
#include <QTimer>
#include <QMetaObject>

Q_LOGGING_CATEGORY(slightshowProjectorManager, "slightshow.projector.manager", QtDebugMsg)

#define for_p(...) \
    for(auto *p : m_projector) { if (p) { __VA_ARGS__; } }

namespace slightshow {

ProjectorManager::ProjectorManager(QObject *parent) :
    QObject(parent), m_serial(new QSerialPort(this)),
    m_projector{new Projector(0, this), new Projector(1, this)},
    m_current(m_projector[1]), m_next(m_projector[0])
{
}

ProjectorManager::~ProjectorManager()
{
    close();
}

bool
ProjectorManager::open(QString port)
{
    for_p( p->stop() );
    m_serial->close();

    m_serial->setPortName(port);
    m_serial->setBaudRate(9600);
    if (m_serial->open(QIODevice::ReadWrite))
    {
        m_serial->clear();
        for_p( p->setPort(m_serial) );
        qCInfo(slightshowProjectorManager) << "Opening USB-SERIAL" << port;
        return true;
    }
    qCCritical(slightshowProjectorManager) << "Cannot open port!" << port;
    return false;
}

void
ProjectorManager::close()
{
    qCDebug(slightshowProjectorManager) << "Closing port" << m_serial->portName();
    for_p( p->stop() );
    m_serial->close();
}

Projector*
ProjectorManager::projector(quint8 index) const
{
    if (index >= 2) return nullptr;
    return m_projector[index];
}

void
ProjectorManager::setMaximumBrightness(qreal brightness)
{
    qCDebug(slightshowProjectorManager) << "Limiting global brightness to:" << brightness;
    for_p( p->setBrightnessScalar(brightness) );
}

bool
ProjectorManager::forward(qreal time)
{
    if (m_is_moving) return false;
    m_is_moving = true;
    qCDebug(slightshowProjectorManager) << "Moving forward";

    m_current->fade(0, time);
    m_next->fade(1, time);

    auto conn = std::make_shared<QMetaObject::Connection>();
    *conn = connect(m_current, &Projector::fadeFinished, [this, conn]()
    {
        QObject::disconnect(*conn);

        qCDebug(slightshowProjectorManager) << "Sliding forward";
        m_current->slide(Projector::Direction::Forward);

        auto conn2 = std::make_shared<QMetaObject::Connection>();
        *conn2 = connect(m_current, &Projector::slideFinished, [this, conn2](Projector::Direction)
        {
            QObject::disconnect(*conn2);

            emit forwardFinished();
            qCDebug(slightshowProjectorManager) << "Finished forward move";

            std::swap(m_next, m_current);
            m_is_moving = false;
        });
    });
    return true;
}

bool
ProjectorManager::backward(qreal time)
{
    if (m_is_moving) return false;
    m_is_moving = true;

    qCDebug(slightshowProjectorManager) << "Moving backward";

    m_next->slide(Projector::Direction::Backward);

    auto conn = std::make_shared<QMetaObject::Connection>();
    *conn = connect(m_next, &Projector::slideFinished, [this, conn, time](Projector::Direction)
    {
        QObject::disconnect(*conn);

        qCDebug(slightshowProjectorManager) << "Fading backward";
        m_current->fade(0, time);
        m_next->fade(1, time);

        auto conn2 = std::make_shared<QMetaObject::Connection>();
        *conn2 = connect(m_next, &Projector::fadeFinished, [this, conn2]()
        {
            QObject::disconnect(*conn2);

            emit backwardFinished();
            qCDebug(slightshowProjectorManager) << "Finished backward move";

            std::swap(m_next, m_current);
            m_is_moving = false;
        });
    });
    return true;
}

} // namespace slightshow
