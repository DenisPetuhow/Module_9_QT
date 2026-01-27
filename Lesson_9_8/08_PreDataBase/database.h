#ifndef DATABASE_H
#define DATABASE_H

#include <QTableWidget>
#include <QObject>

// Qt SQL модуль - разные варианты подключения
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtSql>
#else
#include <QtSql/QtSql>
#endif

// Драйвер PostgreSQL и имя соединения
#define POSTGRE_DRIVER "QPSQL"
#define DB_NAME "MyDB"

//Количество полей данных необходимых для подключения к БД
#define NUM_DATA_FOR_CONNECT_TO_DB 5

//Перечисление полей данных
enum fieldsForConnect{
    hostName = 0,
    dbName = 1,
    login = 2,
    pass = 3,
    port = 4
};

//Типы запросов
enum requestType{

    requestAllFilms = 1,  // Все фильмы - QSqlTableModel
    requestComedy   = 2,  // Комедии - QSqlQueryModel
    requestHorrors  = 3   // Ужасы - QSqlQueryModel

};



class DataBase : public QObject  //Управляет соединением с БД и моделями данных.
{
    Q_OBJECT

public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    // Методы управления соединением
    void AddDataBase(QString m_driver, QString m_nameDB = "");
    void ConnectToDataBase(QVector<QString> m_dataForConnect);
    void DisconnectFromDataBase(QString m_nameDb = "");

    // Метод выполнения запроса
    void RequestToDB(int m_requestType);

    // Получение последней ошибки
    QSqlError GetLastError(void);


signals:
    // Сигнал передачи модели с данными во View
    void sig_SendDataFromDB(QSqlQueryModel* m_model, int m_typeRequest);

    // Сигнал статуса подключения
    void sig_SendStatusConnection(bool m_status);



private:

    QSqlDatabase* __dataBase;      // Указатель на объект соединения с БД
    // Модели для разных типов запросов
    QSqlTableModel* __tableModel;  // Для всех фильмов
    QSqlQueryModel* __queryModel;  // Для фильтрованных запросов

};

#endif // DATABASE_H
