#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "UdpWorker.h"

#define TIMER_DELAY 1000

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pb_start_clicked();
    void on_pb_stop_clicked();
    void on_pb_sendDatagram_clicked();     // Слот обработки нажатия кнопки отправки сообщения

    // Слоты обработки сигналов от воркера
    void DisplayTime(QDateTime m_data);
    void DisplayTextInfo(QHostAddress m_address, int m_size);     //  Слот для отображения информации о пакете
private:
    Ui::MainWindow *ui;
    QTimer* timer;
    UDPworker* __udpWorker;
    uint32_t __counterPck = 0;

    void AddToLog(const QString& m_message);



};
#endif // MAINWINDOW_H
