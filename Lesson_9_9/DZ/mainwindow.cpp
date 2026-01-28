#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    __udpWorker = new UDPworker(this);
    __udpWorker->InitSocket();

    connect(__udpWorker, &UDPworker::sig_sendTimeToGUI, this, &MainWindow::DisplayTime);\
    connect(__udpWorker, &UDPworker::sig_sendTextInfoToGUI, this, &MainWindow::DisplayTextInfo);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]{

        QDateTime dateTime = QDateTime::currentDateTime();

        QByteArray dataToSend;
        QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

        outStr << dateTime;

        __udpWorker->SendTimeData(dateTime);
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pb_start_clicked()
{
    timer->start(TIMER_DELAY);
}


void MainWindow::DisplayTime(QDateTime m_data)
{
    // Формируем сообщение только про время
    QString msg = "Текущее время: " + m_data.toString();
    AddToLog(msg);
}

void MainWindow::DisplayTextInfo(QHostAddress m_address, int m_size)
{
    // Формируем сообщение только про адрес и размер
    QString msg = QString("Принято сообщение от %1, размер: %2 байт")
                      .arg(m_address.toString())
                      .arg(m_size);
    AddToLog(msg);
}

void MainWindow::AddToLog(const QString& m_message)
{
    __counterPck++;

    if(__counterPck % 20 == 0){
        ui->te_result->clear();
    }

    // Добавляем счетчик к сообщению для наглядности (как было в исходнике)
    ui->te_result->append(m_message + ". Пакет №" + QString::number(__counterPck));
}

void MainWindow::on_pb_stop_clicked()
{
    timer->stop();
}

void MainWindow::on_pb_sendDatagram_clicked()
{
    QString text = ui->le_textToSend->text();
    if(text.isEmpty()) return;

    // Используем специализированный метод для текста
    __udpWorker->SendTextData(text.toUtf8());
}

