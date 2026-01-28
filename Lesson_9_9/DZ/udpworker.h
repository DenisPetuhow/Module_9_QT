#ifndef UDPWORKER_H
#define UDPWORKER_H

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDateTime>


#define BIND_PORT 12345

enum DatagramType {
    TimeType = 1,
    TextType = 2
};


class UDPworker : public QObject
{
    Q_OBJECT
public:
    explicit UDPworker(QObject *parent = nullptr);
    void InitSocket( void );
    // Методы для отправки конкретных типов данных
    void SendTimeData(const QDateTime& m_dateTime);
    void SendTextData(const QByteArray& m_data);


private slots:
    void readPendingDatagrams(void);

private:
    QUdpSocket* __serviceUdpSocket;
    void SendDatagram(const QByteArray& m_data);

signals:
    void sig_sendTimeToGUI(QDateTime m_data);
    void sig_sendTextInfoToGUI(QHostAddress m_address, int m_size); // Сигнал для передачи информации об адресе и размере

};

#endif // UDPWORKER_H
