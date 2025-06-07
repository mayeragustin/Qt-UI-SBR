#include "qtuisbr.h"
#include "ui_qtuisbr.h"

QtUISBR::QtUISBR(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::QtUISBR)
{
    ui->setupUi(this);
}

QtUISBR::~QtUISBR()
{
    delete ui;
}
