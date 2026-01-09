#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "signals.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pb_sendSignal_clicked();

public slots:
    void ReceiveSignal();
    void ReceiveSignalString(QString str);
    void ReceiveSignalNum(int num);

private:
    Ui::MainWindow *ui;
    Signals *sig;
};
#endif // MAINWINDOW_H
