# Методичка: Работа с PostgreSQL в Qt

## Приложение "Каталог фильмов"

---

## 1. Что изменилось: Прекод → Итоговый код

### 1.1. CMakeLists.txt

```cmake
# БЫЛО (прекод):
find_package(Qt6 REQUIRED COMPONENTS Widgets)
target_link_libraries(... Qt6::Widgets ...)

# СТАЛО (итог):
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Concurrent)
target_link_libraries(... Qt6::Widgets Qt6::Sql Qt6::Concurrent ...)
```

| Модуль | Назначение |
|--------|------------|
| `Qt6::Sql` | Классы для работы с БД (QSqlDatabase, QSqlTableModel, QSqlQueryModel) |
| `Qt6::Concurrent` | Асинхронное выполнение (QtConcurrent::run) |

---

### 1.2. mainwindow.ui

```xml
<!-- БЫЛО (прекод): -->
<widget class="QTableWidget" name="tb_result">

<!-- СТАЛО (итог): -->
<widget class="QTableView" name="tb_result">
```

| Виджет | Архитектура | Особенности |
|--------|-------------|-------------|
| `QTableWidget` | Item-based | Хранит данные внутри, ручное заполнение |
| `QTableView` | Model/View | Отображает данные из модели, автоматическое обновление |

---

### 1.3. database.h

```cpp
// БЫЛО (прекод):
#include <QTableWidget>
// Сигнал с QTableWidget
void sig_SendDataFromDB(const QTableWidget *tableWg, int typeR);

private:
    QSqlDatabase* dataBase;

// СТАЛО (итог):
#include <QtSql>
// Сигнал с QSqlQueryModel
void sig_SendDataFromDB(QSqlQueryModel* m_model, int m_typeRequest);

private:
    QSqlDatabase* __dataBase;
    QSqlTableModel* __tableModel;  // Для "Все фильмы"
    QSqlQueryModel* __queryModel;  // Для "Комедии"/"Ужасы"
```

---

### 1.4. database.cpp — Подключение к БД

```cpp
// БЫЛО (прекод):
void DataBase::ConnectToDataBase(QVector<QString> data)
{
    // ... установка параметров ...
    
    ///Тут должен быть код ДЗ
    
    bool status;
    status = dataBase->open();
    emit sig_SendStatusConnection(status);
}

// СТАЛО (итог):
void DataBase::ConnectToDataBase(QVector<QString> m_data)
{
    __dataBase->setHostName(m_data[hostName]);
    __dataBase->setDatabaseName(m_data[dbName]);
    __dataBase->setUserName(m_data[login]);
    __dataBase->setPassword(m_data[pass]);
    __dataBase->setPort(m_data[port].toInt());

    bool status = __dataBase->open();
    
    // ВАЖНО: Модели НЕ создаём здесь!
    // Этот метод выполняется в ДРУГОМ потоке (QtConcurrent::run)
    // Qt SQL объекты не потокобезопасны
    
    emit sig_SendStatusConnection(status);
}
```

---

### 1.5. database.cpp — Выполнение запросов

```cpp
// БЫЛО (прекод):
void DataBase::RequestToDB(QString request)
{
    ///Тут должен быть код ДЗ
}

// СТАЛО (итог):
void DataBase::RequestToDB(int m_requestType)
{
    QSqlDatabase db = QSqlDatabase::database(DB_NAME);
    
    if (!db.isOpen()) return;

    switch(m_requestType) {
    
    case requestAllFilms: {
        // Пересоздаём модель (в главном потоке!)
        if (__tableModel != nullptr) delete __tableModel;
        
        __tableModel = new QSqlTableModel(this, db);
        __tableModel->setTable("film");
        __tableModel->select();  // Выполняет SELECT * FROM film
        
        __tableModel->setHeaderData(1, Qt::Horizontal, "Название фильма");
        __tableModel->setHeaderData(2, Qt::Horizontal, "Описание фильма");
        
        emit sig_SendDataFromDB(__tableModel, requestAllFilms);
        break;
    }
    
    case requestComedy: {
        if (__queryModel != nullptr) delete __queryModel;
        
        __queryModel = new QSqlQueryModel(this);
        
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
    
    case requestHorrors: {
        // Аналогично Comedy, но WHERE c.name = 'Horror'
        // ...
    }
    }
}
```

---

### 1.6. mainwindow.h

```cpp
// БЫЛО (прекод):
void ScreenDataFromDB(const QTableWidget *widget, int typeRequest);

// СТАЛО (итог):
void ScreenDataFromDB(QSqlQueryModel *m_model, int m_typeRequest);
```

---

### 1.7. mainwindow.cpp — Отображение данных

```cpp
// БЫЛО (прекод):
void MainWindow::ScreenDataFromDB(const QTableWidget *widget, int typeRequest)
{
    ///Тут должен быть код ДЗ
}

// СТАЛО (итог):
void MainWindow::ScreenDataFromDB(QSqlQueryModel *m_model, int m_typeRequest)
{
    // Привязываем модель к View
    ui->tb_result->setModel(m_model);
    
    // Сбрасываем скрытие столбцов от предыдущего запроса
    for (int i = 0; i < m_model->columnCount(); ++i) {
        ui->tb_result->showColumn(i);
    }
    
    // Для "Все фильмы" скрываем лишние столбцы
    if (m_typeRequest == requestAllFilms) {
        ui->tb_result->hideColumn(0);  // film_id
        for (int i = 3; i < m_model->columnCount(); ++i) {
            ui->tb_result->hideColumn(i);
        }
    }
    
    ui->tb_result->resizeColumnsToContents();
    ui->tb_result->horizontalHeader()->setStretchLastSection(true);
}
```

---

### 1.8. mainwindow.cpp — Обработка кнопки "Получить"

```cpp
// БЫЛО (прекод):
void MainWindow::on_pb_request_clicked()
{
    ///Тут должен быть код ДЗ
}

// СТАЛО (итог):
void MainWindow::on_pb_request_clicked()
{
    int currentIndex = ui->cb_category->currentIndex();
    int typeRequest;
    
    switch (currentIndex) {
    case 0: typeRequest = requestAllFilms; break;
    case 1: typeRequest = requestComedy;   break;
    case 2: typeRequest = requestHorrors;  break;
    default: typeRequest = requestAllFilms; break;
    }
    
    __dataBase->RequestToDB(typeRequest);
}
```

---

## 2. Архитектура Model/View

### 2.1. Схема взаимодействия компонентов

```
┌─────────────────────────────────────────────────────────────────────┐
│                           ПОЛЬЗОВАТЕЛЬ                              │
└─────────────────────────────────────────────────────────────────────┘
                                   │
                                   ▼
┌─────────────────────────────────────────────────────────────────────┐
│                          MainWindow (UI)                            │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────────────┐  │
│  │ QComboBox    │  │ QPushButton  │  │      QTableView          │  │
│  │ cb_category  │  │ pb_request   │  │      tb_result           │  │
│  │              │  │  "Получить"  │  │                          │  │
│  │ • Все        │  └──────┬───────┘  │  ┌────────┬───────────┐  │  │
│  │ • Комедия    │         │          │  │ Назв.  │ Описание  │  │  │
│  │ • Ужасы      │         │          │  ├────────┼───────────┤  │  │
│  └──────────────┘         │          │  │ ...    │ ...       │  │  │
│                           │          │  └────────┴───────────┘  │  │
└───────────────────────────┼──────────┴──────────┼───────────────┘
                            │                     │
                            │ RequestToDB(type)   │ setModel(model)
                            ▼                     │
┌─────────────────────────────────────────────────┼───────────────────
│                       DataBase                  │
│  ┌─────────────────────────────────────────────────────────────┐
│  │                    QSqlDatabase                             │
│  │                    (соединение с PostgreSQL)                │
│  └─────────────────────────────────────────────────────────────┘
│                              │
│         ┌────────────────────┼────────────────────┐
│         ▼                    ▼                    ▼
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐
│  │ QSqlTableModel  │  │ QSqlQueryModel  │  │ QSqlQueryModel  │
│  │ (Все фильмы)    │  │ (Комедии)       │  │ (Ужасы)         │
│  │                 │  │                 │  │                 │
│  │ SELECT * FROM   │  │ SELECT ... JOIN │  │ SELECT ... JOIN │
│  │ film            │  │ WHERE 'Comedy'  │  │ WHERE 'Horror'  │
│  └────────┬────────┘  └────────┬────────┘  └────────┬────────┘
│           │                    │                    │
│           └────────────────────┼────────────────────┘
│                                │
│                    sig_SendDataFromDB(model, type)
└────────────────────────────────┼────────────────────────────────────
                                 │
                                 ▼
                          QTableView отображает данные из модели
```

---

### 2.2. Последовательность действий

```
┌──────────────────────────────────────────────────────────────────────┐
│ 1. ПОДКЛЮЧЕНИЕ К БД                                                  │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   Пользователь              MainWindow              DataBase         │
│        │                         │                      │            │
│        │── Меню "Подключиться" ─►│                      │            │
│        │                         │                      │            │
│        │                         │── QtConcurrent::run ─┼──┐         │
│        │                         │                      │  │         │
│        │                         │                      │  ▼         │
│        │                         │                      │ ConnectTo  │
│        │                         │                      │ DataBase() │
│        │                         │                      │  │         │
│        │                         │                      │  │ open()  │
│        │                         │                      │  │         │
│        │                         │◄─ sig_SendStatus ────┼──┘         │
│        │                         │   Connection(true)   │            │
│        │                         │                      │            │
│        │◄─ "Подключено к БД" ────│                      │            │
│        │   (зелёный статус)      │                      │            │
│                                                                      │
└──────────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────────┐
│ 2. ЗАПРОС ДАННЫХ                                                     │
├──────────────────────────────────────────────────────────────────────┤
│                                                                      │
│   Пользователь              MainWindow              DataBase         │
│        │                         │                      │            │
│        │── Выбор "Комедия" ─────►│                      │            │
│        │── Клик "Получить" ─────►│                      │            │
│        │                         │                      │            │
│        │                         │── RequestToDB ──────►│            │
│        │                         │   (requestComedy)    │            │
│        │                         │                      │            │
│        │                         │                      │ new QSql   │
│        │                         │                      │ QueryModel │
│        │                         │                      │     │      │
│        │                         │                      │ setQuery() │
│        │                         │                      │     │      │
│        │                         │◄─ sig_SendDataFromDB─┤            │
│        │                         │   (model, type)      │            │
│        │                         │                      │            │
│        │                         │ ScreenDataFromDB()   │            │
│        │                         │ setModel(model)      │            │
│        │                         │                      │            │
│        │◄─ Таблица с комедиями ──│                      │            │
│                                                                      │
└──────────────────────────────────────────────────────────────────────┘
```

---

## 3. Основные классы Qt SQL

### 3.1. QSqlDatabase — Соединение с БД

```cpp
// Регистрация соединения
QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", "MyConnection");
//                                          ▲          ▲
//                                      драйвер    имя соединения

// Параметры подключения
db.setHostName("server.example.com");
db.setDatabaseName("mydb");
db.setUserName("user");
db.setPassword("password");
db.setPort(5432);

// Открытие соединения
bool success = db.open();

// Получение соединения по имени (из любого места программы)
QSqlDatabase db = QSqlDatabase::database("MyConnection");

// Проверка состояния
db.isOpen();   // true если соединение открыто
db.isValid();  // true если драйвер загружен

// Закрытие
db.close();
```

**Доступные драйверы:**

| Драйвер | БД |
|---------|-----|
| `QPSQL` | PostgreSQL |
| `QMYSQL` | MySQL / MariaDB |
| `QSQLITE` | SQLite |
| `QODBC` | ODBC (универсальный) |

---

### 3.2. QSqlTableModel — Модель одной таблицы

**Назначение:** Работа с одной таблицей, поддержка редактирования.

```cpp
// Создание (ВАЖНО: передаём соединение!)
QSqlTableModel *model = new QSqlTableModel(parent, QSqlDatabase::database("MyDB"));

// Указание таблицы
model->setTable("film");

// Фильтрация (аналог WHERE)
model->setFilter("release_year > 2000");

// Сортировка (аналог ORDER BY)
model->setSort(1, Qt::AscendingOrder);  // По столбцу 1, по возрастанию

// Выполнение запроса
model->select();  // Автоматически: SELECT * FROM film WHERE ... ORDER BY ...

// Заголовки столбцов
model->setHeaderData(0, Qt::Horizontal, "ID");
model->setHeaderData(1, Qt::Horizontal, "Название");

// Получение данных
int rowCount = model->rowCount();
int colCount = model->columnCount();
QString value = model->data(model->index(0, 1)).toString();
```

**Стратегии редактирования:**

```cpp
model->setEditStrategy(QSqlTableModel::OnFieldChange);   // Сохранение при изменении поля
model->setEditStrategy(QSqlTableModel::OnRowChange);     // Сохранение при смене строки
model->setEditStrategy(QSqlTableModel::OnManualSubmit);  // Ручное сохранение

// При OnManualSubmit:
model->submitAll();  // Сохранить все изменения
model->revertAll();  // Отменить все изменения
```

---

### 3.3. QSqlQueryModel — Модель произвольного запроса

**Назначение:** Выполнение сложных SQL-запросов (JOIN, GROUP BY), только чтение.

```cpp
// Создание
QSqlQueryModel *model = new QSqlQueryModel(parent);

// Выполнение произвольного SQL
model->setQuery("SELECT title, description FROM film WHERE rating = 'PG'", 
                QSqlDatabase::database("MyDB"));

// Сложный запрос с JOIN
QString query = "SELECT f.title, c.name "
               "FROM film f "
               "JOIN film_category fc ON f.film_id = fc.film_id "
               "JOIN category c ON c.category_id = fc.category_id";
model->setQuery(query, db);

// Заголовки (индексы соответствуют порядку в SELECT)
model->setHeaderData(0, Qt::Horizontal, "Фильм");
model->setHeaderData(1, Qt::Horizontal, "Категория");

// Проверка ошибок
if (model->lastError().isValid()) {
    qDebug() << "Ошибка:" << model->lastError().text();
}
```

---

### 3.4. Сравнение QSqlTableModel и QSqlQueryModel

| Характеристика | QSqlTableModel | QSqlQueryModel |
|----------------|----------------|----------------|
| **Источник данных** | Одна таблица | Произвольный SQL |
| **JOIN** | ❌ Нет | ✅ Да |
| **Редактирование** | ✅ Да | ❌ Нет |
| **Метод запроса** | `setTable()` + `select()` | `setQuery()` |
| **Фильтрация** | `setFilter()` | В SQL-запросе |
| **Сортировка** | `setSort()` | В SQL-запросе |
| **Наследование** | Наследует QSqlQueryModel | Базовый класс |

---

### 3.5. QTableView — Виджет отображения

```cpp
// Привязка модели
ui->tableView->setModel(model);

// Управление столбцами
ui->tableView->hideColumn(0);           // Скрыть столбец
ui->tableView->showColumn(0);           // Показать столбец
ui->tableView->setColumnWidth(1, 200);  // Ширина столбца

// Автоподстройка размеров
ui->tableView->resizeColumnsToContents();
ui->tableView->resizeRowsToContents();

// Растянуть последний столбец
ui->tableView->horizontalHeader()->setStretchLastSection(true);

// Выделение строк целиком
ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

// Чередование цвета строк
ui->tableView->setAlternatingRowColors(true);

// Очистка (отвязка модели)
ui->tableView->setModel(nullptr);
```

---

## 4. Обработка ошибок

### 4.1. Ошибки подключения

```cpp
void DataBase::ConnectToDataBase(QVector<QString> m_data)
{
    // ... настройка параметров ...
    
    bool status = __dataBase->open();
    
    if (!status) {
        // Ошибка сохраняется в объекте БД
        // Можно получить через GetLastError()
    }
    
    emit sig_SendStatusConnection(status);
}

QSqlError DataBase::GetLastError()
{
    return __dataBase->lastError();
}
```

### 4.2. Отображение ошибки пользователю

```cpp
void MainWindow::ReceiveStatusConnectionToDB(bool m_status)
{
    if (m_status) {
        // Успешное подключение
        ui->lb_statusConnect->setText("Подключено к БД");
        ui->lb_statusConnect->setStyleSheet("color:green");
        ui->pb_request->setEnabled(true);
    }
    else {
        // Ошибка подключения
        __dataBase->DisconnectFromDataBase(DB_NAME);
        
        // Показываем диалог с ошибкой
        __msg->setIcon(QMessageBox::Critical);
        __msg->setText(__dataBase->GetLastError().text());
        __msg->exec();
        
        ui->lb_statusConnect->setText("Отключено");
        ui->lb_statusConnect->setStyleSheet("color:red");
    }
}
```

### 4.3. Класс QSqlError

```cpp
QSqlError error = database->lastError();
// или
QSqlError error = model->lastError();

// Методы QSqlError:
error.isValid();       // true если есть ошибка
error.text();          // Полный текст ошибки
error.databaseText();  // Текст от драйвера БД
error.driverText();    // Текст от Qt драйвера
error.type();          // Тип ошибки (enum)

// Типы ошибок:
QSqlError::NoError           // Нет ошибки
QSqlError::ConnectionError   // Ошибка соединения
QSqlError::StatementError    // Ошибка SQL-запроса
QSqlError::TransactionError  // Ошибка транзакции
QSqlError::UnknownError      // Неизвестная ошибка
```

### 4.4. Проверка ошибок в запросах

```cpp
void DataBase::RequestToDB(int m_requestType)
{
    // Для QSqlTableModel
    bool success = __tableModel->select();
    if (!success) {
        qDebug() << "Ошибка select():" << __tableModel->lastError().text();
        return;
    }
    
    // Для QSqlQueryModel
    __queryModel->setQuery(query, db);
    if (__queryModel->lastError().isValid()) {
        qDebug() << "Ошибка запроса:" << __queryModel->lastError().text();
        return;
    }
}
```

---

## 5. Многопоточность и Qt SQL

### 5.1. Проблема

Qt SQL объекты **не потокобезопасны**. Нельзя создавать объект в одном потоке, а использовать в другом.

```cpp
// ❌ НЕПРАВИЛЬНО:
void DataBase::ConnectToDataBase(...)  // Вызывается в потоке QtConcurrent
{
    bool status = __dataBase->open();
    if (status) {
        // Создание в РАБОЧЕМ потоке
        __tableModel = new QSqlTableModel(this, db);
    }
}

void DataBase::RequestToDB(...)  // Вызывается в ГЛАВНОМ потоке
{
    // Использование в ДРУГОМ потоке — ОШИБКА!
    __tableModel->select();
}
```

### 5.2. Решение

Создавать SQL-объекты там же, где они используются:

```cpp
// ✅ ПРАВИЛЬНО:
void DataBase::ConnectToDataBase(...)  // Рабочий поток
{
    bool status = __dataBase->open();
    // Модели НЕ создаём здесь!
    emit sig_SendStatusConnection(status);
}

void DataBase::RequestToDB(...)  // Главный поток
{
    // Создание и использование в ОДНОМ потоке
    __tableModel = new QSqlTableModel(this, db);
    __tableModel->select();
    emit sig_SendDataFromDB(__tableModel, type);
}
```

### 5.3. Правило

```
┌─────────────────────────────────────────────────────────────┐
│  QSqlDatabase::open() — можно в любом потоке                │
│  QSqlTableModel / QSqlQueryModel — создавать и использовать │
│  только в ОДНОМ потоке (желательно в главном UI потоке)     │
└─────────────────────────────────────────────────────────────┘
```

---

## 6. SQL-запрос с JOIN — разбор

```sql
SELECT f.title, f.description 
FROM film f 
JOIN film_category fc ON f.film_id = fc.film_id 
JOIN category c ON c.category_id = fc.category_id 
WHERE c.name = 'Comedy'
```

### Структура таблиц

```
┌─────────────────┐     ┌─────────────────┐     ┌─────────────────┐
│      film       │     │  film_category  │     │    category     │
├─────────────────┤     ├─────────────────┤     ├─────────────────┤
│ film_id (PK)    │◄───┐│ film_id (FK)    │┌───►│ category_id (PK)│
│ title           │    └┤ category_id (FK)├┘    │ name            │
│ description     │     └─────────────────┘     └─────────────────┘
│ ...             │      (связующая таблица)
└─────────────────┘
```

### Разбор по частям

| Часть | Описание |
|-------|----------|
| `SELECT f.title, f.description` | Выбираем только 2 столбца |
| `FROM film f` | Из таблицы film (псевдоним f) |
| `JOIN film_category fc ON f.film_id = fc.film_id` | Соединяем со связующей таблицей |
| `JOIN category c ON c.category_id = fc.category_id` | Соединяем с таблицей категорий |
| `WHERE c.name = 'Comedy'` | Фильтруем по названию категории |

---

## 7. Структура итогового проекта

```
08_PreDataBase/
├── CMakeLists.txt          # Конфигурация сборки
├── main.cpp                # Точка входа
├── mainwindow.h            # Заголовок главного окна
├── mainwindow.cpp          # Реализация главного окна
├── mainwindow.ui           # UI главного окна (QTableView)
├── database.h              # Заголовок класса БД
├── database.cpp            # Реализация работы с БД
├── dbdata.h                # Заголовок диалога подключения
├── dbdata.cpp              # Реализация диалога
├── dbdata.ui               # UI диалога подключения
└── PG_Libs/                # Библиотеки PostgreSQL (Windows)
    ├── libpq.dll
    ├── libcrypto-3-x64.dll
    └── ...
```

---

## 8. Краткая шпаргалка

```cpp
// 1. Подключение к БД
QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", "MyDB");
db.setHostName("host");
db.setDatabaseName("database");
db.setUserName("user");
db.setPassword("pass");
db.setPort(5432);
db.open();

// 2. Простой запрос (одна таблица)
QSqlTableModel *model = new QSqlTableModel(this, QSqlDatabase::database("MyDB"));
model->setTable("film");
model->select();

// 3. Сложный запрос (JOIN)
QSqlQueryModel *model = new QSqlQueryModel(this);
model->setQuery("SELECT ... JOIN ... WHERE ...", QSqlDatabase::database("MyDB"));

// 4. Отображение
ui->tableView->setModel(model);

// 5. Проверка ошибок
if (model->lastError().isValid()) {
    qDebug() << model->lastError().text();
}
```

---

## 9. Частые ошибки и решения

| Ошибка | Причина | Решение |
|--------|---------|---------|
| `Driver not loaded` | Не установлен PostgreSQL или не прописан PATH | Установить PostgreSQL, добавить bin и lib в PATH |
| `QSqlDatabase does not name a type` | Не подключён модуль Sql | Добавить `#include <QtSql>` и `Qt6::Sql` в CMake |
| `setModel is private` | Используется QTableWidget вместо QTableView | Заменить виджет в .ui файле |
| Данные не отображаются | Модель создана в другом потоке | Создавать модели в главном потоке |
| `multiple definition` | Включён .cpp вместо .h | Проверить #include директивы |
