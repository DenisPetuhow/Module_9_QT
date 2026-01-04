#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_rb_pr_toggled(bool checked);

    void on_rb_day_toggled(bool checked);

    void on_cb_bar_currentIndexChanged(int index);

    // Слот, который будет срабатывать каждую секунду
    void on_timer_tick();

    void on_bt_push_clicked(bool checked); // тип toggel

private:
    Ui::MainWindow *ui;
    // Внутренняя переменная для хранения текущего прогресса
    int m_counter;
    QTimer *m_timer; // Указатель на таймер
};
#endif // MAINWINDOW_H
