#include "mainwindow.h"
#include "ui_mainwindow.h" // Генерируется автоматически из .ui
#include "stopwatch.h"

MainWindow::MainWindow(QWidget *m_parent) :
    QMainWindow(m_parent),
    __ui(new Ui::MainWindow)
{
    __ui->setupUi(this); // Инициализация виджетов из дизайнера
    __stopwatch = new Stopwatch(this);     // Инициализация логики

    // -- Настройка начального состояния UI --
    __ui->pb_start->setText("Старт");
    __ui->pb_clear->setText("Очистить");
    __ui->pb_time->setText("Круг");
    __ui->pb_time->setEnabled(false);     // Блокируем кнопку круга до старта
    __ui->le_time->setReadOnly(true);     // QLineEdit (le_time) делаем только для чтения
    __ui->le_time->setText("0.0");
    // -- Подключение сигналов (Observer pattern) --

    // Логика -> Обновление UI
    connect(__stopwatch, &Stopwatch::sig_timeUpdated, this, &MainWindow::slot_updateDisplay);
    connect(__stopwatch, &Stopwatch::sig_lapRecorded, this, &MainWindow::slot_appendLap);
}

MainWindow::~MainWindow()
{
    delete __ui;
}

void MainWindow::on_pb_start_clicked()
{
    if (__stopwatch->isRunning()) {
        __stopwatch->stop();
        __ui->pb_start->setText("Старт");
        __ui->pb_time->setEnabled(false); // Блокируем круг
    } else {
        __stopwatch->start();
        __ui->pb_start->setText("Стоп");
        __ui->pb_time->setEnabled(true);  // Разрешаем круг
    }
}

void MainWindow::on_pb_clear_clicked()
{
    __stopwatch->reset();
    __ui->tb_race->clear(); // Очищаем QTextBrowser
    __ui->pb_start->setText("Старт");
    __ui->pb_time->setEnabled(false);
}


void MainWindow::on_pb_time_clicked()
{
    __stopwatch->recordLap();
}

// Обновление поля le_time
void MainWindow::slot_updateDisplay(double m_time)
{
    __ui->le_time->setText(QString::number(m_time, 'f', 1));
}

// Добавление записи в tb_race
void MainWindow::slot_appendLap(int m_n, double m_time, double m_totalTime)
{
    QString msg = QString("Круг %1, время: %2 сек, общее время: %3")
                      .arg(m_n)
                      .arg(m_time, 0, 'f', 1)
                      .arg(m_totalTime, 0, 'f', 1);

    __ui->tb_race->append(msg);
}
