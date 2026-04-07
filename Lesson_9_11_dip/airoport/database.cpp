#include "database.h"
#include <QDebug>
#include <QThread>

// ═══════════════════════════════════════════════════════════════
// Конструктор — настраивает таймер переподключения
// ═══════════════════════════════════════════════════════════════
Database::Database(QObject *parent)
    : QObject(parent)
{
    __connectionName = "AirportInspectorDB"; // Уникальное имя соединения. Qt использует его как ключ для хранения соединений во внутреннем реестре.
    __reconnectTimer = new QTimer(this);
    __reconnectTimer->setInterval(5000);
    __reconnectTimer->setSingleShot(true);     // singleShot = true: срабатывает один раз, а не циклично. вызываем когда надо
    connect(__reconnectTimer, &QTimer::timeout,this,&Database::connectToDatabase);     // Связываем таймер с повторной попыткой подключения.
}

Database::~Database()
{

    if (QSqlDatabase::contains(__connectionName)) {     // Проверяем, существует ли соединение с таким именем
        // Получаем объект соединения по имени и закрываем
        {
            QSqlDatabase db = QSqlDatabase::database(__connectionName);
            db.close();
        }
        QSqlDatabase::removeDatabase(__connectionName);         // после обязательно удаляем removeDatabase() нужно вызывать ПОСЛЕ того,
    }
}

void Database::connectToDatabase() // connectToDatabase — подключается к PostgreSQL (ФТ1)
{
    qDebug() << "Database::connectToDatabase() — попытка подключения..."
             << "Поток:" << QThread::currentThread();

    // ── Шаг 1: Создаём или переиспользуем соединение ──
    if (QSqlDatabase::contains(__connectionName)) { //QSqlDatabase::contains() проверяет, есть ли уже соединение с таким именем в реестре Qt. Если да — используем его.Если нет — создаём новое с драйвером QPSQL (PostgreSQL).

        __db = QSqlDatabase::database(__connectionName);
    } else {
        __db = QSqlDatabase::addDatabase("QPSQL", __connectionName); // QPSQL — плагин драйвера PostgreSQL для Qt.Чтобы он работал, в системе должна быть установленабиблиотека libpq (клиент PostgreSQL). На Windows — нужен libpq.dll рядом с exe или в PATH.
    }

    // ── Шаг 2: Задаём параметры подключения ──
    __db.setHostName(DB_HOST);
    __db.setPort(DB_PORT);
    __db.setDatabaseName(DB_NAME);
    __db.setUserName(DB_USER);
    __db.setPassword(DB_PASSWORD);

    // ── Шаг 3: Пытаемся открыть соединение ──

    if (__db.open()) {    // open() возвращает true при успехе, false при ошибке.
        __reconnectTimer->stop();
        qDebug() << "Database::connectToDatabase() — ПОДКЛЮЧЕНО";
        emit sig_connected();         // Сигнал для MainWindow: обновить статус, разблокировать UI

    } else {

        // Подключение не удалось
        QString errorText = __db.lastError().text();   // текст ошибки от драйвера // При ошибке lastError() содержит описание проблемы.
        qDebug() << "Database::connectToDatabase() — ОШИБКА:" << errorText;
        emit sig_connectError(errorText);        // Сигнал для MainWindow: показать QMessageBox (ФТ5)
        __reconnectTimer->start();         // Запускаем таймер: через 5 секунд попробуем снова (ФТ6)
    }
}

void Database::requestAirports() // запрос списка аэропортов из БД (ФТ7)
{
    // ── Шаг 1: Создаём и выполняем SQL-запрос ──
    QSqlQuery query(__db);
    bool success = query.exec(
        "SELECT airport_name->>'ru' AS \"airportName\", airport_code "
        "FROM bookings.airports_data "
        "ORDER BY \"airportName\""
        );
    // ── Шаг 2: Проверяем результат ──
    if (!success) {
        qDebug() << "Database::requestAirports() — ОШИБКА:"<< query.lastError().text();
        emit sig_disconnected();        // Сообщаем MainWindow о потере соединения
        __reconnectTimer->start();     // Пытаемся переподключиться через 5 секунд
        return;
    }
    // ── Шаг 3: Извлекаем данные из результата запроса ──
    QList<QPair<QString, QString>> airports; // создаем список
    while (query.next()) {  // query.next() перемещает курсор на следующую строку результата. Возвращает false, когда строки закончились.
        QString name = query.value(0).toString();   // query.value(0) — первый столбец "Шереметьево"
        QString code = query.value(1).toString();    // "SVO"
        airports.append(qMakePair(name, code)); // создаем
    }
    qDebug() << "Database::requestAirports() — получено аэропортов:" << airports.size();
    emit sig_airportsReady(airports); //Сигнал передаёт КОПИЮ списка (QList копируется при передаче).

}
