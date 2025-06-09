#ifndef QTUISBR_H
#define QTUISBR_H

#include "sensorchart.h"
#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>
#include <QLCDNumber>

#include <QtCore>
#include <QtGui>
#include <QtCharts/QtCharts>

#define RAD_TO_DEG 57.295779513f

#define LMotor          Motors[0]
#define RMotor          Motors[1]

typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
}sAxis;

typedef struct{
    sAxis Acc;
    sAxis Gyro;
    struct{
        float Acc_pitch;
        float Acc_roll;
        float pitch;
        float roll;
        float yaw;
    }Angle;
}s_MPU6050;

typedef struct{
    uint16_t pps;
    float rpm;
}s_Encoder;

typedef struct{
    uint32_t Speed;
    uint8_t Direction;
    s_Encoder Encoder;
    struct{
        uint16_t radius;
    }Morph;
}s_Motor;

typedef struct{
    uint16_t data[4];
    struct{
        uint32_t vertice;
    }Parabola;
}s_LineFollower;

typedef struct CAR_VARIABLES{
    s_MPU6050 MPU6050;
    s_Motor Motors[2];
    s_LineFollower LineFollower;
}sCar;


typedef union{
    uint8_t     u8[4];
    int8_t      i8[4];
    uint16_t    u16[2];
    int16_t     i16[2];
    uint32_t    u32;
    int32_t     i32;
    float       f;
}_work;


QT_BEGIN_NAMESPACE
namespace Ui {
class QtUISBR;
}
QT_END_NAMESPACE

class QtUISBR : public QMainWindow
{
    Q_OBJECT

public:
    QtUISBR(QWidget *parent = nullptr);
    ~QtUISBR();

    void OnQTimer1();
    /*void on_pushButton_open_clicked();*/
    void OnRxChar();
    void DecodeCmd(uint8_t *rxBuf);
    bool eventFilter(QObject *watched, QEvent *event);
    void on_pushButton_send_clicked();
    void SendCMD(uint8_t *buf, uint8_t length);
private slots:
    void on_pushButton_open_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_clear_2_clicked();

    void on_lineEdit_editingFinished();

    void on_lineEdit_2_editingFinished();

    void on_verticalSlider_sliderReleased();

    void on_verticalSlider_2_sliderReleased();

private:
    Ui::QtUISBR *ui;

    QTimer *QTimer1;

    _work decom;

    QSerialPort *QSerialPort1;
    uint8_t rxBuf[256], header = 0, nBytes, cks, index, tmoRX, buffer[24], cmdIndex=0;
    char FIRMVERSION[15];
    typedef enum{
        UNKNOWNCOMANND= 0xFF,
        ACKNOWLEDGE=    0x0D,
        ALIVE=          0xF0,
        FIRMWARE=       0xF1,
        PULSADORES=     0x12,
        ADCSINGLE=      0xA0,
        ADCBLOCK=       0xA1,
        MOTOR=          0xA2,
        GET_ENCODER=    0xA3,
        DEBUG=          0xDE,
        USERTEXT=       0xB1,
        USERNUMBER=     0xB2,
        SYSERROR=       0xEE,
        MPUBLOCK=       0xA4
    }_eIDCommand;

    sCar Car;

    uint32_t t = 300, DT = 100;
    double dt = 0;
    clock_t current_time = 0, last_time = 0;

    //Kalman *roll;
   // Kalman *pitch;
    float gyroRateY = 0, gyroRateX = 0;

    QLCDNumber *ADCLCDlist[8];


    SensorChart *grafico;
    time_t graphtimer = 0;

};
#endif // QTUISBR_H
