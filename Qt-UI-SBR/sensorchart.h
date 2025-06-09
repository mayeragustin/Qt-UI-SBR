#ifndef SENSORCHART_H
#define SENSORCHART_H

#include <QWidget>
#include <QMap>
#include <QString>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>

class SensorChart : public QWidget
{
    Q_OBJECT

public:
    explicit SensorChart(QWidget *parent = nullptr);
    ~SensorChart();

    void agregarSensor(const QString &nombre, const QColor &color = Qt::blue);
    void actualizarSensor(const QString &nombre, qreal valor, qreal tiempo);

    void setMaxDuration(int segundos); // Para cambiar los segundos visibles (default: 60)


    void setRangoEjeY(qreal min, qreal max);

    void eliminarSensor(const QString &nombre);

    void mostrarSensor(const QString &nombre, bool mostrar);
private:
    QChart *chart;
    QChartView *chartView;

    QMap<QString, QLineSeries*> sensoresMap;
    QValueAxis *axisX;
    QValueAxis *axisY;

    int maxDuration = 60; // segundos
    qreal x = 0; // tiempo actual
};

#endif // SENSORCHART_H
