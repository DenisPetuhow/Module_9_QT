#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <QObject>
#include <QTimer>

class Stopwatch : public QObject
{
    Q_OBJECT // Макрос для работы сигналов и слотов
public:
    explicit Stopwatch(QObject *m_parent = nullptr);
    ~Stopwatch();


    // Структура для хранения данных круга
    struct LapData {
        int lapNumber;
        double lapTime;
        double totalTime;
    };


    // -- Методы управления --
    void start();       // Запуск таймера
    void stop();        // Остановка таймера
    void reset();       // Сброс данных
    LapData recordLap();   // Логика расчета круга
    bool isRunning() const; // Получение текущего статуса

signals:
    void sig_timeUpdated(double m_time);
    void sig_lapRecorded(int m_lapNumber, double m_lapTime, double m_totaltime);     // Сигнал: сообщает о фиксации круга

private slots:
    // Внутренний слот, вызываемый таймером
    void slot_onTimerTimeout();

private:
    QTimer *__timer;                  // Объект таймера Qt
    long long __totalTimeDeciseconds; // Общее время (в 0.1 сек)
    long long __lastLapTimeDeciseconds; // Время последнего круга (в 0.1 сек)
    int __lapCounter;                 // Счётчик кругов
    bool __isRunning;                 // Флаг: идет ли отсчет
};
#endif // STOPWATCH_H
