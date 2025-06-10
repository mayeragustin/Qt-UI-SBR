/**
 * @file SensorChart.h
 * @brief Clase para graficar múltiples sensores en tiempo real utilizando Qt Charts
 *
 * Esta clase permite mostrar múltiples series de datos de sensores en un gráfico dinámico.
 * Cada sensor puede tener su propio color, rango de visualización y visibilidad individual.
 *
 * @author Agustín Alejandro Mayer
 */

#ifndef SENSORCHART_H
#define SENSORCHART_H

#include <QWidget>
#include <QMap>
#include <QString>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>

    /**
 * @class SensorChart
 * @brief Widget personalizado para graficar múltiples señales de sensores en tiempo real
 *
 * Permite:
 * - Agregar sensores con nombre y color
 * - Actualizar sus valores en tiempo real
 * - Configurar duración del historial (ej: últimos 60 segundos)
 * - Ajustar el rango del eje Y
 * - Mostrar/ocultar o eliminar sensores dinámicamente
 *
 * @example Ejemplo básico de uso en Qt Creator:
 * @code
 * // En tu ventana principal (ej: MainWindow)
 * #include "sensorchart.h"
 *
 * // Promueve un QWidget a SensorChart desde Qt Designer
 * SensorChart *grafico = ui->widgetGrafico;
 *
 * // Configura el rango del eje Y
 * grafico->setRangoEjeY(-100, 100);
 *
 * // Agrega sensores (inicialmente invisibles)
 * grafico->agregarSensor("Temperatura", Qt::red);
 * grafico->agregarSensor("Humedad", Qt::blue);
 *
 * // Conecta checkboxes para mostrar/ocultar
 * connect(ui->checkBox_temperatura, &QCheckBox::toggled, [=](bool checked){
 *     grafico->mostrarSensor("Temperatura", checked);
 * });
 *
 * connect(ui->checkBox_humedad, &QCheckBox::toggled, [=](bool checked){
 *     grafico->mostrarSensor("Humedad", checked);
 * });
 *
 * // Timer para actualizar datos
 * QTimer *timer = new QTimer(this);
 * connect(timer, &QTimer::timeout, this, [=](){
 *     static qreal t = 0;
 *     qreal temp = qrand() % 100;
 *     qreal humedad = qrand() % 100;
 *
 *     grafico->actualizarSensor("Temperatura", temp, t);
 *     grafico->actualizarSensor("Humedad", humedad, t);
 *
 *     t += 1;
 * });
 *
 * timer->start(1000); // cada segundo
 * @endcode
 */
    class SensorChart : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor de la clase SensorChart
     * @param parent Puntero al widget padre (por defecto es nullptr)
     */
    explicit SensorChart(QWidget *parent = nullptr);

    /**
     * @brief Destructor de la clase SensorChart
     */
    ~SensorChart();

    /**
     * @brief Agrega un nuevo sensor al gráfico
     * @param nombre Nombre único del sensor
     * @param color Color de la línea del sensor (por defecto es azul)
     */
    void agregarSensor(const QString &nombre, const QColor &color = Qt::blue);

    /**
     * @brief Actualiza el valor de un sensor existente
     * @param nombre Nombre del sensor
     * @param valor Nuevo valor del sensor
     * @param tiempo Tiempo actual (en segundos) asociado al valor
     */
    void actualizarSensor(const QString &nombre, qreal valor, qreal tiempo);

    /**
     * @brief Establece la duración máxima de datos mostrados en el gráfico
     * @param segundos Duración en segundos (por defecto es 60)
     */
    void setMaxDuration(int segundos); // Para cambiar los segundos visibles (default: 60)

    /**
     * @brief Establece el rango del eje Y (valores del sensor)
     * @param min Valor mínimo del eje Y
     * @param max Valor máximo del eje Y
     */
    void setRangoEjeY(qreal min, qreal max);

    /**
     * @brief Elimina completamente un sensor del gráfico
     * @param nombre Nombre del sensor a eliminar
     */
    void eliminarSensor(const QString &nombre);

    /**
     * @brief Muestra u oculta una serie del gráfico
     * @param nombre Nombre del sensor
     * @param mostrar true para mostrar, false para ocultar
     */
    void mostrarSensor(const QString &nombre, bool mostrar);

private:
    QChart *chart;                   ///< Gráfico base
    QChartView *chartView;           ///< Vista del gráfico

    QMap<QString, QLineSeries*> sensoresMap; ///< Mapea nombres a series
    QValueAxis *axisX;               ///< Eje X (tiempo)
    QValueAxis *axisY;               ///< Eje Y (valor del sensor)

    int maxDuration = 60;            ///< Duración del historial en segundos
    qreal x = 0;                     ///< Tiempo actual en segundos
};

#endif // SENSORCHART_H
