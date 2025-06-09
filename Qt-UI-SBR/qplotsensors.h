#ifndef QPLOTSENSORS_H
#define QPLOTSENSORS_H

#include <QObject>
#include <QWidget>

class QPlotSensors : public QObject
{
    Q_OBJECT
public:
    explicit QPlotSensors(QObject *parent = nullptr);

signals:
};

#endif // QPLOTSENSORS_H
