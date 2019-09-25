#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>
#include <QString>

namespace slightshow {

struct ConfigSeries
{
    QString *name;
};

class Configuration : public QObject
{
    Q_OBJECT
public:
    explicit Configuration(QObject *parent = nullptr);
    explicit Configuration(QString *path, QObject *parent = nullptr);


signals:

public slots:
};

} // namespace slightshow

#endif // CONFIGURATION_H
