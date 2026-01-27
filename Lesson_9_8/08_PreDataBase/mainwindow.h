#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtConcurrent>

// Для QSqlQueryModel
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtSql>
#else
#include <QtSql/QtSql>
#endif

#include "database.h"
#include "dbdata.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void ScreenDataFromDB(QSqlQueryModel *m_model, int m_typeRequest); //Слот для отображения данных из БД
    void ReceiveStatusConnectionToDB(bool status);  // Слот обработки статуса подключения к БД


private slots:
    void on_act_addData_triggered(); // Обработчик пункта меню "Ввести данные"
    void on_act_connect_triggered();     // Обработчик пункта меню "Подключиться/Отключиться"
    void on_pb_request_clicked();     // Обработчик кнопки "Получить"
    // Обработчик кнопки "Очистить"
    void on_pb_clear_clicked();

signals:
    void sig_RequestToDb(QString request);

private:

    QVector<QString> __dataForConnect; //Данные для подключения к БД.

    Ui::MainWindow *ui;
    DbData *__dataDb;       // Диалог ввода данных подключения
    DataBase *__dataBase;   // Объект работы с БД
    QMessageBox *__msg;     // Диалог сообщений об ошибках

    QString request = "SELECT title, release_year, c.name  FROM film f "
                      "JOIN film_category fc on f.film_id = fc.film_id "
                      "JOIN category c on c.category_id  = fc.category_id";


};
#endif // MAINWINDOW_H
