#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>

/// Данные подключения к БД (ФТ2:

static const QString DB_HOST     = "981757-ca08998.tmweb.ru";
static const int     DB_PORT     = 5432;
static const QString DB_NAME     = "demo";
static const QString DB_USER     = "netology_usr_cpp";
static const QString DB_PASSWORD = "CppNeto3";

/**
Управление подключением к PostgreSQL
 *
 * Принцип работы: 1. Объект создаётся в MainWindow2. Перемещается в отдельный QThread (moveToThread)3. Все слоты выполняются в контексте рабочего потока 4. Общение с MainWindow — через сигналы/слоты (потокобезопасно)
 *
 * Почему отдельный поток:
 *   Подключение к удалённой БД может занять несколько секунд.Если делать это в главном потоке — интерфейс «замёрзнет».
 * ВАЖНО: QSqlDatabase нужно создавать и использовать в одном потоке.
 */
class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

public slots:

    void connectToDatabase(); // Подключается к БД PostgreSQL (ФТ1) Создаёт QSqlDatabase с драйвером QPSQL, задаёт параметры
    /**
     *   SELECT airport_name->>'ru' AS "airportName", airport_code
     *   FROM bookings.airports_data
     *   ORDER BY "airportName"
     */ //Запрашивает список аэропортов из БД (ФТ7)      * Результат передаётся как QList<QPair<QString, QString>>:first  = название аэропорта на русском  second = код аэропорта
    void requestAirports();  //QSqlQuery привязан к QSqlDatabase, которая живёт в рабочем потоке. Передавать QSqlQuery между потоками небезопасно. поэтому QPair

signals:

    void sig_connected();     /// Успешное подключение к БД — MainWindow разблокирует UI
    void sig_connectError(QString m_error);
    void sig_disconnected(); // заблокирует UI и обновит статус
    void sig_airportsReady(QList<QPair<QString, QString>> airports); //  Список аэропортов готов (ФТ7) MainWindow получит этот сигнал и заполнит QComboBox.
private:
    QSqlDatabase __db;            // объект подключения к PostgreSQL
    QTimer *__reconnectTimer; // Таймер повторного подключения (ФТ6) sleep() блокирует поток целиком — нельзя обрабатывать другие сигналы.  QTimer работает через event loop — поток остаётся живым.
    QString __connectionName; // Имя соединения с БД Qt позволяет иметь несколько одновременных соединений с разными БД. По умолчанию используется "qt_sql_default_connection",
};

#endif // DATABASE_H
