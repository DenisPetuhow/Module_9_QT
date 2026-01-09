#include "stopwatch.h"

// Конструктор: инициализируем переменные нулями
Stopwatch::Stopwatch(QObject *m_parent) : QObject(m_parent)
{
    __timer = new QTimer(this);
    __totalTimeDeciseconds = 0;
    __lastLapTimeDeciseconds = 0;
    __lapCounter = 0;
    __isRunning = false;

    // Связываем системный таймер с нашим методом обработки
    connect(__timer, &QTimer::timeout, this, &Stopwatch::slot_onTimerTimeout);
}

Stopwatch::~Stopwatch()
{
    // QTimer удалится автоматически благодаря parent-child механизму Qt
}

void Stopwatch::start()
{
    __isRunning = true;
    __timer->start(100); // Тик каждые 100 мс (0.1 сек)
}

void Stopwatch::stop()
{
    __isRunning = false;
    __timer->stop();
}

void Stopwatch::reset()
{
    stop(); // Останавливаем перед сбросом
    __totalTimeDeciseconds = 0;
    __lastLapTimeDeciseconds = 0;
    __lapCounter = 0;

    // Принудительно отправляем 0 в интерфейс
    emit sig_timeUpdated(0.0);
}

void Stopwatch::recordLap()
{
    __lapCounter++;
    // Считаем разницу между текущим общим временем и временем прошлого круга
    long long currentLap = __totalTimeDeciseconds - __lastLapTimeDeciseconds;
    __lastLapTimeDeciseconds = __totalTimeDeciseconds;

    // Отправляем данные (делим на 10.0 для получения секунд)
    emit sig_lapRecorded(__lapCounter, currentLap / 10.0, __totalTimeDeciseconds / 10.0);
}

bool Stopwatch::isRunning() const
{
    return __isRunning;
}

void Stopwatch::slot_onTimerTimeout()
{
    __totalTimeDeciseconds++;
    // Уведомляем подписчиков (UI) о новом времени
    emit sig_timeUpdated(__totalTimeDeciseconds / 10.0); // делим на 10 так как таймер срабатывает каждую 0.1 сек, чтобы корректно отображать
}
