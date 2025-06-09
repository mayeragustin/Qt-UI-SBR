#include "sensorchart.h"
#include "qboxlayout.h"

SensorChart::SensorChart(QWidget *parent)
    : QWidget(parent)
{
    // Configurar gráfico
    chart = new QChart();
    chart->setTitle("Sensores en Tiempo Real");

    axisX = new QValueAxis;
    axisX->setTitleText("Tiempo (s)");
    axisX->setRange(0, maxDuration);
    axisX->setLabelFormat("%.1f");
    axisX->setTickCount(11);

    axisY = new QValueAxis;
    axisY->setTitleText("Valor");
    axisY->setRange(0, 100);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
}

SensorChart::~SensorChart()
{
    qDeleteAll(sensoresMap);
    sensoresMap.clear();
}

void SensorChart::setMaxDuration(int segundos)
{
    maxDuration = segundos;
    axisX->setRange(0, segundos);
}

void SensorChart::agregarSensor(const QString &nombre, const QColor &color)
{
    if (sensoresMap.contains(nombre))
        return;

    QLineSeries *serie = new QLineSeries(this);
    serie->setName(nombre);
    serie->setColor(color);
    serie->setVisible(false);

    chart->addSeries(serie);
    serie->attachAxis(axisX);
    serie->attachAxis(axisY);

    sensoresMap[nombre] = serie;
}

void SensorChart::actualizarSensor(const QString &nombre, qreal valor, qreal tiempo)
{
    if (!sensoresMap.contains(nombre))
        return;

    QLineSeries *serie = sensoresMap[nombre];

    // Agregar nuevo punto con el tiempo dado
    serie->append(tiempo, valor);

    // Eliminar puntos fuera del rango
    while (serie->count() > 0 && (tiempo - serie->at(0).x()) > maxDuration) {
        serie->removePoints(0, 1);
    }

    // Actualizar eje X dinámicamente
    if (tiempo >= maxDuration) {
        axisX->setRange(tiempo - maxDuration, tiempo);
    } else {
        axisX->setRange(0, maxDuration);
    }
}

void SensorChart::setRangoEjeY(qreal min, qreal max)
{
    axisY->setRange(min, max);
}

void SensorChart::eliminarSensor(const QString &nombre)
{
    if (!sensoresMap.contains(nombre))
        return;

    QLineSeries *serie = sensoresMap.take(nombre);
    chart->removeSeries(serie);
    delete serie;
}

void SensorChart::mostrarSensor(const QString &nombre, bool mostrar)
{
    if (!sensoresMap.contains(nombre))
        return;

    QLineSeries *serie = sensoresMap[nombre];

    if (mostrar) {
        serie->setVisible(true);
    } else {
        serie->setVisible(false);
    }
}
