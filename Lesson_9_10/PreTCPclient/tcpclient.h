#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QDateTime>
#include <QHostAddress>
#include <QMessageBox>
#include "structs.h" // Подключаем общий файл со структурами

class TCPclient : public QObject
{
    Q_OBJECT
public:
    explicit TCPclient(QObject *parent = nullptr);
    // Методы управления соединением
    void ConnectToHost(QHostAddress host, uint16_t port);
    void DisconnectFromHost(void);

    // Методы отправки запросов
    // SendRequest - для простых команд (получить время, статистику)
    void SendRequest(ServiceHeader head);
    // SendData - для отправки строки (SET_DATA)
    void SendData(ServiceHeader head, QString data);



private slots:

    void ReadyReed(void); // Слот обработки входящих данных от сокета


private:

QTcpSocket* __socket;
ServiceHeader servHeader;
void ProcessingData(ServiceHeader header, QDataStream &stream);

signals:
// Сигналы для обновления интерфейса (MainWindow)
void sig_sendTime(QDateTime time);          // Получено время
void sig_sendStat(StatServer stat);         // Получена статистика
void sig_sendFreeSize(uint32_t size);       // Получен размер своб. места (доп.)
void sig_SendReplyForSetData(QString rep);  // Ответ на отправку строки (доп.)
void sig_Success(uint16_t type);            // Успешное выполнение команды (доп.)

void sig_connectStatus(uint16_t status);    // Статус соединения (успех/ошибка)
void sig_Disconnected(void);                // Разрыв соединения
void sig_Error(uint16_t code);              // Ошибка от сервера

};

#endif // TCPCLIENT_H
