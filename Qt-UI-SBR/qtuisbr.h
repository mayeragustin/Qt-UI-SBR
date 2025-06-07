#ifndef QTUISBR_H
#define QTUISBR_H

#include <QMainWindow>

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

private:
    Ui::QtUISBR *ui;
};
#endif // QTUISBR_H
