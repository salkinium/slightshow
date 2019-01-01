#ifndef PROJECTORMANAGER_H
#define PROJECTORMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QLoggingCategory>
#include "projector.h"
#include <functional>

Q_DECLARE_LOGGING_CATEGORY(slightshowProjectorManager)

namespace slightshow {

class ProjectorManager : public QObject
{
    Q_OBJECT
public:
    explicit ProjectorManager(QObject *parent = nullptr);
    ~ProjectorManager();

    bool
    open(QString port);

    void
    close();

signals:
    void
    forwardFinished();

    void
    backwardFinished();

public slots:
    void
    setMaximumBrightness(qreal brightness);

    bool
    forward(qreal time);

    bool
    backward(qreal time);

private:
    QSerialPort *const m_serial;
    Projector *const m_projector[2];

    Projector *m_current;
    Projector *m_next;

    bool m_is_moving = false;
};

} // namespace slightshow

#endif // PROJECTORMANAGER_H
