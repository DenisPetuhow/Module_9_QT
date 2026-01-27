#include "database.h"

DataBase::DataBase(QObject *m_parent)
    : QObject{m_parent}
{
    // Инициализация указателя на базу и обнуление моделей
    __dataBase = new QSqlDatabase();
    __tableModel = nullptr;
    __queryModel = nullptr;
}

DataBase::~DataBase()
{
    delete __dataBase;
}

// Регистрация драйвера БД (PostgreSQL) с заданным именем подключения
void DataBase::AddDataBase(QString m_driver, QString m_nameDB)
{
    *__dataBase = QSqlDatabase::addDatabase(m_driver, m_nameDB);
}

// Настройка параметров и открытие соединения
void DataBase::ConnectToDataBase(QVector<QString> m_data)
{
    __dataBase->setHostName(m_data[hostName]);
    __dataBase->setDatabaseName(m_data[dbName]);
    __dataBase->setUserName(m_data[login]);
    __dataBase->setPassword(m_data[pass]);
    __dataBase->setPort(m_data[port].toInt());

    // Попытка открытия соединения
    bool status = __dataBase->open();

    // Отправка результата подключения (успех/неудача) в GUI
    emit sig_SendStatusConnection(status);
}

// Закрытие соединения с БД
void DataBase::DisconnectFromDataBase(QString m_nameDb)
{
    *__dataBase = QSqlDatabase::database(m_nameDb);
    __dataBase->close();
}

// Обработка запросов на получение данных
void DataBase::RequestToDB(int m_requestType)
{
    QSqlDatabase db = QSqlDatabase::database(DB_NAME);

    // Если соединения нет — выходим
    if (!db.isOpen()) {
        return;
    }

    switch(m_requestType) {

    // 1. Запрос "Все фильмы" — используем QSqlTableModel (прямая работа с таблицей)
    case requestAllFilms: {
        if (__tableModel != nullptr) {
            delete __tableModel;
        }
        __tableModel = new QSqlTableModel(this, db);
        __tableModel->setTable("film");       // Указываем таблицу
        __tableModel->select();               // Загружаем данные
        //setTable() — имя таблицы
        //setFilter() — условие WHERE
        //setSort() — сортировка ORDER BY

        // Установка заголовков для столбцов
        __tableModel->setHeaderData(1, Qt::Horizontal, "Название фильма");
        __tableModel->setHeaderData(2, Qt::Horizontal, "Описание фильма");

        // Отправка готовой модели в GUI
        emit sig_SendDataFromDB(__tableModel, requestAllFilms);
        break;
    }

    // 2. Запрос "Комедии" — используем QSqlQueryModel (SQL-запрос)
    case requestComedy: {
        if (__queryModel != nullptr) {
            delete __queryModel;
        }
        __queryModel = new QSqlQueryModel(this);

        // SQL запрос с объединением таблиц для фильтрации по категории
        QString query = "SELECT f.title, f.description "
                        "FROM film f "
                        "JOIN film_category fc ON f.film_id = fc.film_id "
                        "JOIN category c ON c.category_id = fc.category_id "
                        "WHERE c.name = 'Comedy'";

        __queryModel->setQuery(query, db);
        __queryModel->setHeaderData(0, Qt::Horizontal, "Название фильма");
        __queryModel->setHeaderData(1, Qt::Horizontal, "Описание фильма");

        emit sig_SendDataFromDB(__queryModel, requestComedy);
        break;
    }

    // 3. Запрос "Ужасы" — аналогично комедиям
    case requestHorrors: {
        if (__queryModel != nullptr) {
            delete __queryModel;
        }
        __queryModel = new QSqlQueryModel(this);

        QString query = "SELECT f.title, f.description "
                        "FROM film f "
                        "JOIN film_category fc ON f.film_id = fc.film_id "
                        "JOIN category c ON c.category_id = fc.category_id "
                        "WHERE c.name = 'Horror'";

        __queryModel->setQuery(query, db);
        __queryModel->setHeaderData(0, Qt::Horizontal, "Название фильма");
        __queryModel->setHeaderData(1, Qt::Horizontal, "Описание фильма");

        emit sig_SendDataFromDB(__queryModel, requestHorrors);
        break;
    }

    default:
        break;
    }
}

// Возврат последней ошибки (для вывода в GUI)
QSqlError DataBase::GetLastError()
{
    return __dataBase->lastError();
}
