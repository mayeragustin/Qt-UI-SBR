#include "qtuisbr.h"
#include "ui_qtuisbr.h"


#define ALPHA 0.98


QtUISBR::QtUISBR(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QtUISBR)
{
    ui->setupUi(this);

    QSerialPort1 = new QSerialPort(this);
    ui->comboBox_com->installEventFilter(this);
    connect(QSerialPort1, &QSerialPort::readyRead, this,&QtUISBR::OnRxChar);
    ui->comboBox_command->addItem("ALIVE", 0xF0);
    ui->comboBox_command->addItem("INFO", 0xF3);
    ui->comboBox_command->addItem("IR", 0xA6);
    ui->comboBox_command->addItem("DEBUG", 0xDE);


    QTimer1 = new QTimer(this);
    connect(QTimer1, &QTimer::timeout, this, &QtUISBR::OnQTimer1);
    QTimer1->start(DT);

    memset(&Car, 0, sizeof(sCar));

    QLCDNumber* ADCLCDlist[] = {
        ui->lcdNumber_LF1,
        ui->lcdNumber_LF2,
        ui->lcdNumber_LF3,
        ui->lcdNumber_LF4,
        ui->lcdNumber_WD1,
        ui->lcdNumber_WD2,
        ui->lcdNumber_WD3,
        ui->lcdNumber_WD4
    };
    Q_UNUSED(ADCLCDlist);
}

QtUISBR::~QtUISBR()
{
    delete ui;
}


void QtUISBR::OnQTimer1()
{
    buffer[0] = MPUBLOCK;
    SendCMD(buffer, 1);
}

void QtUISBR::SendCMD(uint8_t *buf, uint8_t length){
    uint8_t tx[24];
    uint8_t cks, i;
    QString strHex;

    tx[0] = 'U';
    tx[1] = 'N';
    tx[2] = 'E';
    tx[3] = 'R';
    tx[4] = length + 1;
    tx[5] = ':';

    memcpy(&tx[6], buf, length);

    cks = 0;
    for (i=0; i<(length+6); i++) {
        cks ^= tx[i];
    }

    tx[i] = cks;

    strHex = "--> 0x";
    for (int i=0; i<length+7; i++) {
        strHex = strHex + QString("%1").arg(tx[i], 2, 16, QChar('0')).toUpper();
    }
    //emit inRxChar(strHex);
    //ui->plainTextEdit->appendPlainText(strHex);
    if(QSerialPort1->isOpen())
        QSerialPort1->write((char *)tx, length+7);

    //emit sendCMDWifi(tx, length+7);
}



void QtUISBR::on_pushButton_open_clicked()
{
    if(QSerialPort1->isOpen()){
        QSerialPort1->close();
        ui->pushButton_open->setText("Open");
    }
    else{
        if(ui->comboBox_com->currentText() == "")
            return;
        QSerialPort1->setPortName(ui->comboBox_com->currentText());
        QSerialPort1->setBaudRate(115200);
        QSerialPort1->setParity(QSerialPort::NoParity);
        QSerialPort1->setDataBits(QSerialPort::Data8);
        QSerialPort1->setStopBits(QSerialPort::OneStop);
        QSerialPort1->setFlowControl(QSerialPort::NoFlowControl);
        if(QSerialPort1->open(QSerialPort::ReadWrite)){
            ui->pushButton_open->setText("Close");
        }else{
            QMessageBox::information(this, "Serial Port", "ERROR: Opening PORT");
        }
    }
}

void QtUISBR::OnRxChar(){
    int count;
    uint8_t *buf;
    QString strHex;

    count = QSerialPort1->bytesAvailable();
    if(count <= 0)
        return;

    buf = new uint8_t[count];
    QSerialPort1->read((char *)buf, count);

    strHex = "<-- 0x";
    for (int a=0; a<count; a++) {
        strHex = strHex + QString("%1").arg(buf[a], 2, 16, QChar('0')).toUpper();
    }
    for (int i=0; i<count; i++) {
        strHex = strHex + QString("%1").arg(buf[i], 2, 16, QChar('0')).toUpper();
        switch(header){
        case 0:
            if(buf[i] == 'U'){
                header = 1;
                tmoRX = 5;

            }
            break;
        case 1:
            if(buf[i] == 'N'){
                header = 2;

            }else{
                header = 0;
                i--;
            }
            break;
        case 2:
            if(buf[i] == 'E')
                header = 3;
            else{
                header = 0;
                i--;
            }
            break;
        case 3:
            if(buf[i] == 'R')
                header = 4;
            else{
                header = 0;
                i--;
            }
            break;
        case 4:
            nBytes = buf[i];
            header = 5;
            break;
        case 5:
            if(buf[i] == ':'){
                header = 6;
                index = 0;
                cks = 'U' ^ 'N' ^ 'E' ^ 'R' ^ ':' ^ nBytes;
            }
            else{
                header = 0;
                i--;
            }
            break;
        case 6:
            nBytes--;
            if(nBytes > 0){
                rxBuf[index++] = buf[i];
                cks ^= buf[i];
            }
            else{
                header = 0;
                if(cks == buf[i])
                    DecodeCmd(rxBuf);
                else
                    ui->plainTextEdit->appendPlainText("ERROR CHECKSUM");
            }
            break;
        }
    }
    delete [] buf;
    ui->plainTextEdit_2->appendPlainText(strHex);
}

void QtUISBR::DecodeCmd(uint8_t *rxBuf){
    _work w;

    switch (rxBuf[0]) {
    case ALIVE:
        if(rxBuf[1] == ACKNOWLEDGE)
            ui->plainTextEdit->appendPlainText("I'M ALIVE");
        break;
    case UNKNOWNCOMANND:
        ui->plainTextEdit->appendPlainText("NO CMD");
        break;
    case FIRMWARE:
        for(uint8_t i=0; i<13; i++){
            FIRMVERSION[i]=rxBuf[i+1];
        }
        ui->lineEdit_firm->setText(QString("%1").arg(FIRMVERSION));
        ui->plainTextEdit->appendPlainText("FIRMWARE");
        break;
    case DEBUG:

        break;
    case ADCSINGLE:
        w.u8[0] = rxBuf[2];
        w.u8[1] = rxBuf[3];
        if(rxBuf[1] < 8){
            ui->plainTextEdit->appendPlainText(QString("ANALOG IN %1: %2").arg(rxBuf[1]). arg(w.u16[0]));
            ADCLCDlist[rxBuf[1]]->display(w.u16[0]);
        }
        if(rxBuf[1] == 8){
            ui->progressBar_battery->setValue((float)w.u16[0] * (100.0f / 4095.0f));
        }
        break;
    case ADCBLOCK:
        break;
    case USERTEXT:
        w.u8[0] = rxBuf[1];
        w.u8[1] = rxBuf[2];
        w.u8[2] = rxBuf[3];
        w.u8[3] = rxBuf[4];

        ui->plainTextEdit->insertPlainText(QString("%1").arg(QChar(w.u32)));
        ui->plainTextEdit->insertPlainText("\n");
        break;
    case USERNUMBER:
        w.u8[0] = rxBuf[1];
        w.u8[1] = rxBuf[2];
        w.u8[2] = rxBuf[3];
        w.u8[3] = rxBuf[4];

        ui->plainTextEdit->appendPlainText(QString("%1").arg(w.i32));
        break;

    case GET_ENCODER:
        w.u8[0] = rxBuf[2];
        w.u8[1] = rxBuf[3];
        if(rxBuf[1] < 2){
            Car.Motors[rxBuf[1]].Encoder.pps = w.u16[0];
            ui->plainTextEdit->appendPlainText(QString("ENCODER %1: %2").arg(rxBuf[1]). arg(Car.Motors[rxBuf[1]].Encoder.pps));
            Car.Motors[rxBuf[1]].Encoder.rpm = Car.Motors[rxBuf[1]].Encoder.pps * 60.0 / 210.0;
            ui->plainTextEdit->appendPlainText(QString("RPM %1: %2\n").arg(rxBuf[1]).arg(Car.Motors[rxBuf[1]].Encoder.rpm));
        }
        break;
    case SYSERROR:
        ui->plainTextEdit->appendPlainText(QString("SYSTEM ERROR: "));
        for(uint8_t i = 0; i < rxBuf[1]; i++){
            ui->plainTextEdit->insertPlainText(QString("%1").arg(QChar(rxBuf[i+2])));
        }
        ui->plainTextEdit->insertPlainText("\n");
        break;
    case MPUBLOCK:
        w.u8[0] = rxBuf[1];
        w.u8[1] = rxBuf[2];
        Car.MPU6050.Acc.x = w.i16[0];
        ui->lcdNumber_accx->display(Car.MPU6050.Acc.x);
        qDebug() << w.i16[0];
        w.u8[0] = rxBuf[3];
        w.u8[1] = rxBuf[4];
        Car.MPU6050.Acc.y = w.i16[0];
        ui->lcdNumber_accy->display(Car.MPU6050.Acc.y);

        w.u8[0] = rxBuf[5];
        w.u8[1] = rxBuf[6];
        Car.MPU6050.Acc.z = w.i16[0];
        ui->lcdNumber_accz->display(Car.MPU6050.Acc.z);

        w.u8[0] = rxBuf[7];
        w.u8[1] = rxBuf[8];
        Car.MPU6050.Gyro.x = w.i16[0];
        ui->lcdNumber_gyrox->display(Car.MPU6050.Gyro.x);

        w.u8[0] = rxBuf[9];
        w.u8[1] = rxBuf[10];
        Car.MPU6050.Gyro.y = w.i16[0];
        ui->lcdNumber_gyroy->display(Car.MPU6050.Gyro.y);

        w.u8[0] = rxBuf[11];
        w.u8[1] = rxBuf[12];
        Car.MPU6050.Gyro.z = w.i16[0];
        ui->lcdNumber_gyroz->display(Car.MPU6050.Gyro.z);

        dt = (double)(clock() - last_time) / CLOCKS_PER_SEC;
        last_time = clock();

        // --- Calcular Roll ---
        Car.MPU6050.Angle.Acc_roll = atan2(Car.MPU6050.Acc.y,
                                           sqrt(Car.MPU6050.Acc.x * Car.MPU6050.Acc.x +
                                                Car.MPU6050.Acc.z * Car.MPU6050.Acc.z)) * RAD_TO_DEG;

        gyroRateX = Car.MPU6050.Gyro.x / 131.07;
    // Car.MPU6050.Angle.roll = roll->getAngle(Car.MPU6050.Angle.Acc_roll, gyroRateX, dt);
        // Usar Gyro.y para Roll
        Car.MPU6050.Angle.roll = ALPHA * (Car.MPU6050.Angle.roll + (Car.MPU6050.Gyro.x / 131.07) * dt) + (1.0 - ALPHA) * Car.MPU6050.Angle.Acc_roll;

        ui->lcdNumber_Roll->display(Car.MPU6050.Angle.roll);

        // --- Calcular Pitch ---
        Car.MPU6050.Angle.Acc_pitch = atan2(-Car.MPU6050.Acc.x,
                                            sqrt(Car.MPU6050.Acc.y * Car.MPU6050.Acc.y +
                                                 Car.MPU6050.Acc.z * Car.MPU6050.Acc.z)) * RAD_TO_DEG;

        gyroRateY = Car.MPU6050.Gyro.y / 131.07;
       // Car.MPU6050.Angle.pitch = pitch->getAngle(Car.MPU6050.Angle.Acc_pitch, gyroRateY, dt);
        // Usar Gyro.x para Pitch

        Car.MPU6050.Angle.pitch = ALPHA * (Car.MPU6050.Angle.pitch + (Car.MPU6050.Gyro.y / 131.07) * dt) + (1.0 - ALPHA) * Car.MPU6050.Angle.Acc_pitch;

        ui->lcdNumber_Pitch->display(Car.MPU6050.Angle.pitch);

        // --- Calcular Yaw (relativo) ---
        // Dividimos acá también si querés, y sin ifs raros
        Car.MPU6050.Angle.yaw += (Car.MPU6050.Gyro.z / 131.07) * dt;

        ui->lcdNumber_Yaw->display(Car.MPU6050.Angle.yaw);

        ui->plainTextEdit->appendPlainText(QString("MPU6050: Data received"));
        break;

    default:
        ui->plainTextEdit->appendPlainText(QString("ERROR: Comando recibido desconocido"));
        break;
    }
}

bool QtUISBR::eventFilter(QObject *watched, QEvent *event){
    if(watched == ui->comboBox_com){
        if (event->type() == QEvent::MouseButtonPress) {
            ui->comboBox_com->clear();
            QSerialPortInfo SerialPortInfo1;
            for(int i=0;i<SerialPortInfo1.availablePorts().count();i++)
                ui->comboBox_com->addItem(SerialPortInfo1.availablePorts().at(i).portName());
            return QMainWindow::eventFilter(watched, event);
        }
        else {
            return false;
        }
    }
    else{
        return QMainWindow::eventFilter(watched, event);
    }
}


void QtUISBR::on_pushButton_send_clicked()/*BOTON DE ENVIO*/
{
    uint8_t cmd, buf[24];
    int n;

    if(ui->comboBox_command->currentText() == "")
        return;

    cmd = ui->comboBox_command->currentData().toInt();
    n = 0;
    switch (cmd) {
    case ALIVE://ALIVE   PC=>MBED 0xF0 ;  MBED=>PC 0xF0 0x0D
        n = 1;
        break;
    case FIRMWARE://FIRMWARE   PC=>MBED 0xF1 ;  MBED=>PC 0xF1 FIRMWARE
        n =1;
        break;
    case DEBUG:
        decom.u16[0] = 45;
        buf[1] = decom.u8[0], buf[2] = decom.u8[1];
        decom.u16[0] = 24;
        buf[3] = decom.u8[0], buf[4] = decom.u8[1];
        n = 5;
        break;
    case ADCSINGLE:
        break;
    default:
        break;
    }

    if(n){
        buf[0] = cmd;
        SendCMD(buf, n);
    }
}
