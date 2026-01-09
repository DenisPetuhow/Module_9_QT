#ifndef SIGNALS_H
#define SIGNALS_H

#include <QObject>
#include <QDebug>

class Signals : public QObject
{
    Q_OBJECT
public:
    explicit Signals(QObject *parent = nullptr); // Указатель на родителя

    void SendSignal();

    ~Signals();


signals:
    void sig_SendSimleSignal();
    void sig_SendString(QString str);
    void sig_SendNum(int num);
};

#endif // SIGNALS_H
