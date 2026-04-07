#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QMessageBox>

#include "database.h"

// ──────────────────────────────────────────────
// Пространство имён Ui генерируется автоматически
// из файла mainwindow.ui утилитой uic.
// Содержит класс Ui::MainWindow со всеми виджетами формы.
//
// Используется forward declaration (предварительное объявление),
// чтобы не включать сгенерированный заголовок ui_mainwindow.h
// в .h-файл — это ускоряет компиляцию.
// ──────────────────────────────────────────────

/**
* Цепочка вызовов при запуске:
 *   1. Конструктор → emit sig_connectToDB()
    *   2. Database::connectToDatabase() → emit sig_connected()
    *   3. MainWindow::onConnected() → emit sig_requestAirports()
    *   4. Database::requestAirports() → emit sig_airportsReady(данные)
*/


namespace Ui {
class MainWindow;
}
//ПОЯСНЕНИЯ//
/**
 *   cb_airport    — QComboBox, выпадающий список аэропортов
 *   de_time       — QDateEdit, выбор даты
 *   rb_out        — QRadioButton, "Вылет"
 *   rb_in         — QRadioButton, "Прилёт"
 *   groupBox      — QGroupBox, "Данные"
 *   groupBox_2    — QGroupBox, "Направление"
 *   pb_search     — QPushButton, "Показать расписание"
 *   pb_statistic  — QPushButton, "Загруженность аэропортов"
 *   tb_flights    — QTableView, таблица рейсов
 *   lb_status     — QLabel, статус подключения к БД
 */

// airports — список пар: { "Шереметьево", "SVO" }
// Как устроен QComboBox с userData:
//   Индекс │ Видимый текст (displayText)  │ Скрытые данные (userData)
//   ───────┼──────────────────────────────┼──────────────────────────
//     0    │ "Абакан"                     │ "ABA"
//   Получить код выбранного аэропорта: QString code = cb_airport->currentData().toString();  // "SVO"
//   Это удобнее, чем хранить отдельный словарь "название → код",потому что userData привязана к конкретному элементу списка.




class MainWindow : public QMainWindow
{
    Q_OBJECT
 signals:
    void sig_connectToDB();      /// Испускается из главного потока, ловится Database в рабочем потоке
    void sig_requestAirports();     // Запрос списка аэропортов → Database::requestAirports()  (ФТ7)

 private slots:
    void onConnected(); // Обработчик успешного подключения к БД
    void onConnectError(QString m_error);
    void onDisconnected();
    void onAirportsReady(QList<QPair<QString, QString>> airports);  // Заполняет QComboBox cb_airport и разблокирует интерфейс. список пар {название_на_русском, код}

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    // Указатель на сгенерированный класс формы.
    Ui::MainWindow *ui;
    void lockUI(); // Вызывается при запуске, до загрузки списка аэропортов.
    void unlockUI(); // Вызывается после успешной загрузки аэропортов из БД.

    Database *__database;     // объект работы с БД (живёт в __dbThread)
    QThread  *__dbThread;     // рабочий поток для Database
};

#endif // MAINWINDOW_H
