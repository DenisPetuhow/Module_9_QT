#include "udpworker.h"

UDPworker::UDPworker(QObject *parent) : QObject(parent)
{



}


/*!
 * @brief Метод инициализирует UDP сервер
 */
void UDPworker::InitSocket()
{

    __serviceUdpSocket = new QUdpSocket(this);
    /*
     * Соединяем присваиваем адрес и порт серверу и соединяем функцию
     * обраотчик принятых пакетов с сокетом
     */
    __serviceUdpSocket->bind(QHostAddress::LocalHost, BIND_PORT);

    connect(__serviceUdpSocket, &QUdpSocket::readyRead, this, &UDPworker::readPendingDatagrams);

}

// Отправка ВРЕМЕНИ: Пишем тип TimeType, затем объект времени
void UDPworker::SendTimeData(const QDateTime& m_dateTime)
{
    QByteArray dataToSend;
    QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

    outStr << static_cast<int>(DatagramType::TimeType); // Заголовок
    outStr << m_dateTime;                               // Данные

    SendDatagram(dataToSend);
}

// Отправка ТЕКСТА: Пишем тип TextType, затем сам текст
void UDPworker::SendTextData(const QByteArray& m_textData)
{
    QByteArray dataToSend;
    QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

    outStr << static_cast<int>(DatagramType::TextType); // Заголовок
    outStr << m_textData;                               // Данные

    SendDatagram(dataToSend);
}

void UDPworker::SendDatagram(const QByteArray& m_data)
{
    __serviceUdpSocket->writeDatagram(m_data, QHostAddress::LocalHost, BIND_PORT);
}

void UDPworker::readPendingDatagrams(void)
{
    while(__serviceUdpSocket->hasPendingDatagrams()){
        QNetworkDatagram datagram = __serviceUdpSocket->receiveDatagram(); //считывает гистрограмму
        QByteArray data = datagram.data();

        QDataStream inStr(&data, QIODevice::ReadOnly); // запускает поток переоформления

        // 1. Сначала читаем тип сообщения
        int type = 0;
        inStr >> type;

        // 2. В зависимости от типа вызываем нужный сигнал
        if (type == DatagramType::TimeType) {
            QDateTime dateTime;
            inStr >> dateTime;
            if(dateTime.isValid()) {
                emit sig_sendTimeToGUI(dateTime);
            }
        }
        else if (type == DatagramType::TextType) {
            // Для текста нам нужно только знать, что он пришел, и его размер
            // Размер полезной нагрузки = общий размер - размер int (заголовка)
            int payloadSize = data.size() - sizeof(int);
            if (payloadSize < 0) payloadSize = 0;

            emit sig_sendTextInfoToGUI(datagram.senderAddress(), payloadSize);
        }
    }
}
