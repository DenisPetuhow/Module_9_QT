#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Forward declaration для класса логики
class Stopwatch;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *m_parent = nullptr);
    ~MainWindow();

private slots:
    void on_pb_start_clicked();
    void on_pb_clear_clicked();
    void on_pb_time_clicked();

    // -- Слоты обновления отображения --
    void slot_updateDisplay(double m_time);
    void slot_appendLap(int m_n, double m_time, double m_totalTime);

private:
    Ui::MainWindow *__ui;
    Stopwatch *__stopwatch;    // Указатель на бизнес-логику

};
#endif // MAINWINDOW_H
