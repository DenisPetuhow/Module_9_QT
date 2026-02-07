#include "tcpclient.h"




/* ServiceHeader
 * Для работы с потоками наши данные необходимо сериализовать.
 * Поскольку типы данных не стандартные перегрузим оператор << Для работы с ServiceHeader
*/
QDataStream &operator >>(QDataStream &out, ServiceHeader &data){ // Чтение заголовка из потока

    out >> data.id;
    out >> data.idData;
    out >> data.status;
    out >> data.len;
    return out;
};
QDataStream &operator <<(QDataStream &in, ServiceHeader &data){ // Запись заголовка в поток

    in << data.id;
    in << data.idData;
    in << data.status;
    in << data.len;

    return in;
};

// Чтение статистики (StatServer) из потока
// В structs.h обычно только объявление, реализацию делаем здесь
QDataStream &operator >>(QDataStream &out, StatServer &data){
    out >> data.incBytes;
    out >> data.sendBytes;
    out >> data.revPck;
    out >> data.sendPck;
    out >> data.workTime;
    out >> data.clients;
    return out;
}



/*
 * Поскольку мы являемся клиентом, инициализацию сокета
 * проведем в конструкторе. Также необходимо соединить
 * сокет со всеми необходимыми нам сигналами.
*/
TCPclient::TCPclient(QObject *parent) : QObject(parent)
{
    __socket = new QTcpSocket(this);
    connect(__socket, &QTcpSocket::readyRead, this, &TCPclient::ReadyReed); // сигнал готовности читать данные
    connect(__socket, &QTcpSocket::connected, this, [&]{  // Обработка статусов подключения
        emit sig_connectStatus(STATUS_SUCCES);
    });

    connect(__socket, &QTcpSocket::errorOccurred, this, [&]{ // Обработка ошибок сокета (передаем общий код ошибки подключения)
        emit sig_connectStatus(ERR_CONNECT_TO_HOST);
    });

    connect(__socket, &QTcpSocket::disconnected, this, &TCPclient::sig_Disconnected);
}


/* write
 * Метод отправляет запрос на сервер. Сериализировать будем
 * при помощи QDataStream
*/
void TCPclient::SendRequest(ServiceHeader m_head) // Отправка "пустого" запроса (только заголовок).
{
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);

    m_head.len = 0; // Для простых запросов длина данных равна 0
    out << m_head;  // Сериализуем заголовок в байты

    __socket->write(buffer);
}

/* write
 * Такой же метод только передаем еще данные.
*/
void TCPclient::SendData(ServiceHeader head, QString str) //Отправка запроса с данными (строкой).
{
    QByteArray dataBuffer;
    QDataStream outStr(&dataBuffer, QIODevice::WriteOnly);
    // Сначала вычисляем размер строки в байтах (UTF-8)
    QByteArray strData = str.toUtf8();
    // В QDataStream запись QString добавляет 4 байта длины перед строкой,
    QByteArray temp;
    QDataStream tempStream(&temp, QIODevice::WriteOnly);
    tempStream << str; // Записали строку во временный буфер

    head.len = temp.size(); // Теперь знаем точный размер полезной нагрузки

    outStr << head; // Пишем заголовок
    outStr << str;  // Пишем строку (QDataStream сам добавит длину строки перед ней)

    __socket->write(dataBuffer);
}

/*
 * \brief Метод подключения к серверу
 */
void TCPclient::ConnectToHost(QHostAddress m_host, uint16_t m_port)
{
    __socket->connectToHost(m_host, m_port);
}
/*
 * \brief Метод отключения от сервера
 */
void TCPclient::DisconnectFromHost()
{
    __socket->disconnectFromHost();
}


void TCPclient::ReadyReed()
{
    QDataStream incStream(__socket);

    // Проверяем статус потока
    if(incStream.status() != QDataStream::Ok) return;

    // Цикл while нужен, чтобы обработать все пакеты, которые могли "склеиться" в буфере
    while(true){

        // ЭТАП 1: Чтение заголовка
        // Если idData == 0, значит мы ожидаем начало нового пакета
        if(servHeader.idData == 0){

            // Проверяем, достаточно ли байт для чтения заголовка
            if(__socket->bytesAvailable() < sizeof(ServiceHeader)){
                return; // Байт мало, ждем следующего вызова readyRead
            }

            incStream >>servHeader;

            // Проверка на валидность ("Магическое число" ID)
            if(servHeader.id != ID){
                // Если пришел мусор, ищем начало следующего валидного пакета
                uint16_t hdr = 0;
                while(__socket->bytesAvailable()){
                    incStream >> hdr;
                    if(hdr == ID){
                        // Нашли заголовок! Дочитываем остальные поля
                        incStream >> servHeader.idData;
                        incStream >> servHeader.status;
                        incStream >> servHeader.len;
                        break;
                    }
                }
            }
        }

        // ЭТАП 2: Чтение данных (Payload)
        // Проверяем, пришли ли все данные, заявленные в заголовке
        if(__socket->bytesAvailable() < servHeader.len){
            return; // Данные еще не пришли полностью. Ждем.
        }

        // Если мы здесь, значит есть Заголовок + Все данные. Обрабатываем!
        ProcessingData(servHeader, incStream);

        // Сбрасываем idData в 0, чтобы на следующей итерации начать чтение нового пакета
        servHeader.idData = 0;
        servHeader.status = 0;
        servHeader.len = 0;
    }
}


/*
 * Остался метод обработки полученных данных. Согласно протоколу
 * мы должны прочитать данные из сообщения и вывести их в ПИ.
 * Поскольку все типы сообщений нам известны реализуем выбор через
 * switch. Реализуем получение времени.
*/

void TCPclient::ProcessingData(ServiceHeader m_header, QDataStream &m_stream)
{
    // Если сервер вернул ошибку
    if(m_header.status != STATUS_SUCCES){
        emit sig_Error(m_header.status);
        // Пропускаем (вычитываем в никуда) данные ошибки, если они есть
        if(m_header.len > 0) {
            // Просто сдвигаем позицию чтения, чтобы очистить буфер
            m_stream.skipRawData(m_header.len);
        }
        return;
    }

    // Разбор успешных ответов
    switch (m_header.idData){
    case GET_TIME: {
        QDateTime time;
        m_stream >> time;
        emit sig_sendTime(time);
        break;
    }
    case GET_STAT: {
        StatServer stat;
        m_stream >> stat; // Используем наш перегруженный operator>>
        emit sig_sendStat(stat);
        break;
    }
    case GET_SIZE: {
        uint32_t freeSpace;
        m_stream >> freeSpace;
        emit sig_sendFreeSize(freeSpace);
        break;
    }
    case SET_DATA: {
        QString reply;
        m_stream >> reply; // Сервер возвращает эхо-строку
        emit sig_SendReplyForSetData(reply);
        break;
    }
    case CLEAR_DATA: {
        emit sig_Success(CLEAR_DATA);
        break;
    }
    }
}
