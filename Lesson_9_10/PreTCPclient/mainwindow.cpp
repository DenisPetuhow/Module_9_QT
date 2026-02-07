#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    __client = new TCPclient(this);
    //Доступность полей по умолчанию
    ui->le_data->setEnabled(false);
    ui->pb_request->setEnabled(false);
    ui->lb_connectStatus->setText("Отключено");
    ui->lb_connectStatus->setStyleSheet("color: red");


    // Статус и ошибки
    connect(__client, &TCPclient::sig_connectStatus, this, &MainWindow::DisplayConnectStatus);
    connect(__client, &TCPclient::sig_Error, this, &MainWindow::DisplayError);
    connect(__client, &TCPclient::sig_Disconnected, this, [&]{
        DisplayConnectStatus(ERR_CONNECT_TO_HOST);

    // Данные
    connect(__client, &TCPclient::sig_sendTime, this, &MainWindow::DisplayTime);
    connect(__client, &TCPclient::sig_sendStat, this, &MainWindow::DisplayStat);
    connect(__client, &TCPclient::sig_sendFreeSize, this, &MainWindow::DisplayFreeSpace);
    connect(__client, &TCPclient::sig_SendReplyForSetData, this, &MainWindow::SetDataReply);
    connect(__client, &TCPclient::sig_Success, this, &MainWindow::DisplaySuccess);

    // Кнопка очистки текстового поля
    connect(ui->pb_clear, &QPushButton::clicked, ui->tb_result, &QTextBrowser::clear);
    });


}

MainWindow::~MainWindow()
{
    delete ui;
}

/*!
 * \brief Группа методо отображения различных данных
 */
void MainWindow::DisplayTime(QDateTime m_time)
{
    // Форматирование по заданию: "01 фев 2001 21:53:12"
    QString strTime = m_time.toString("dd MMM yyyy hh:mm:ss");
    ui->tb_result->append("Серверное время: " + strTime);
}

void MainWindow::DisplayFreeSpace(uint32_t m_freeSpace)
{
    ui->tb_result->append("Свободно места на сервере: " + QString::number(m_freeSpace));
}

void MainWindow::SetDataReply(QString m_reply)
{
    ui->tb_result->append("Ответ сервера (Эхо): " + m_reply);
}

void MainWindow::DisplayStat(StatServer m_stat)
{
    ui->tb_result->append("--- Статистика Сервера ---");
    ui->tb_result->append(QString("Байт принято: %1").arg(m_stat.incBytes));
    ui->tb_result->append(QString("Байт отправлено: %1").arg(m_stat.sendBytes));
    ui->tb_result->append(QString("Пакетов принято: %1").arg(m_stat.revPck));
    ui->tb_result->append(QString("Пакетов отправлено: %1").arg(m_stat.sendPck));
    ui->tb_result->append(QString("Время работы: %1 сек").arg(m_stat.workTime));
    ui->tb_result->append(QString("Клиентов: %1").arg(m_stat.clients));
    ui->tb_result->append("--------------------------");
}
void MainWindow::DisplayError(uint16_t m_error)
{
    switch(m_error){
    case ERR_NO_FREE_SPACE:
        ui->tb_result->append("Ошибка: Недостаточно места на сервере!");
        break;
    case ERR_NO_FUNCT:
        ui->tb_result->append("Ошибка: Попытка отправить пустые данные!");
        break;
    default:
        ui->tb_result->append("Неизвестная ошибка: код " + QString::number(m_error));
    }
}
/*!
 * \brief Метод отображает квитанцию об успешно выполненном сообщениии
 * \param typeMess ИД успешно выполненного сообщения
 */
void MainWindow::DisplaySuccess(uint16_t m_typeMess)
{
    if(m_typeMess == CLEAR_DATA){
        ui->tb_result->append("Память сервера успешно очищена.");
    }
}

/*!
 * \brief Метод отображает статус подключения
 */
void MainWindow::DisplayConnectStatus(uint16_t m_status)
{
    if(m_status == ERR_CONNECT_TO_HOST){
        ui->lb_connectStatus->setText("Отключено");
        ui->lb_connectStatus->setStyleSheet("color: red");
        ui->pb_connect->setText("Подключиться");
        ui->pb_request->setEnabled(false);
        // Разблокируем поля настроек при разрыве
        ui->spB_port->setEnabled(true);
        ui->spB_ip1->setEnabled(true);
        ui->spB_ip2->setEnabled(true);
        ui->spB_ip3->setEnabled(true);
        ui->spB_ip4->setEnabled(true);
        ui->tb_result->append("Соединение разорвано или не установлено.");
    }
    else {
        ui->lb_connectStatus->setText("Подключено");
        ui->lb_connectStatus->setStyleSheet("color: green");
        ui->pb_connect->setText("Отключиться");
        ui->pb_request->setEnabled(true);
        // Блокируем поля настроек во время работы
        ui->spB_port->setEnabled(false);
        ui->spB_ip1->setEnabled(false);
        ui->spB_ip2->setEnabled(false);
        ui->spB_ip3->setEnabled(false);
        ui->spB_ip4->setEnabled(false);
        ui->tb_result->append("Подключение к серверу выполнено успешно.");
    }
}

/*!
 * \brief Обработчик кнопки подключения/отключения
 */
void MainWindow::on_pb_connect_clicked()
{
    if(ui->pb_connect->text() == "Подключиться"){

        uint16_t port = ui->spB_port->value();

        QString ip = ui->spB_ip4->text() + "." +
                     ui->spB_ip3->text() + "." +
                     ui->spB_ip2->text() + "." +
                     ui->spB_ip1->text();

        __client->ConnectToHost(QHostAddress(ip), port);

    }
    else{

        __client->DisconnectFromHost();
    }
}

/*
 * Для отправки сообщения согласно ПИВ необходимо
 * заполнить заголовок и передать его на сервер. В ответ
 * сервер вернет информацию в соответствии с типом сообщения
*/
void MainWindow::on_pb_request_clicked()
{
    ServiceHeader header;
    header.id = ID;
    header.status = STATUS_SUCCES;

    // Определяем тип запроса по индексу ComboBox
    // 0: Время, 1: Место, 2: Статистика, 3: Отправить данные, 4: Очистить
    switch (ui->cb_request->currentIndex()){
    case 0:
        header.idData = GET_TIME;
        __client->SendRequest(header);
        break;
    case 1:
        header.idData = GET_SIZE;
        __client->SendRequest(header);
        break;
    case 2:
        header.idData = GET_STAT;
        __client->SendRequest(header);
        break;
    case 3:
        header.idData = SET_DATA;
        // Для отправки данных используем спец. метод, который считает длину строки
        __client->SendData(header, ui->le_data->text());
        break;
    case 4:
        header.idData = CLEAR_DATA;
        __client->SendRequest(header);
        break;
    default:
        ui->tb_result->append("Неизвестная команда");
    }
}

/*!
 * \brief Обработчик изменения индекса запроса
 */
void MainWindow::on_cb_request_currentIndexChanged(int index)
{
    //Разблокируем поле отправления данных только когда выбрано "Отправить данные"
    if(ui->cb_request->currentIndex() == 3){
        ui->le_data->setEnabled(true);
    }
    else{
        ui->le_data->setEnabled(false);
    }
}

