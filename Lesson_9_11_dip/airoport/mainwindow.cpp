#include "mainwindow.h"
#include "ui_mainwindow.h"  // Сгенерированный из mainwindow.ui



// ═══════════════════════════════════════════════════════════════
// Конструктор
// ═══════════════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)   // Создаём объект формы
{
    // setupUi() выполняет всю работу, которую мы раньше делали вручную:— создаёт все виджеты, объявленные в .ui  — размещает их по layout'ам — устанавливает свойства (текст, размеры, ограничения дат и т.д.)  — подключает сигналы, заданные в Designer
    ui->setupUi(this);
    lockUI();     // Блокируем интерфейс до подключения к БД (ФТ4)

    __database = new Database();       // объект работы с БД     // Database создаётся БЕЗ родителя . Причина: moveToThread() запрещён для объектов, у которых есть parent.
    __dbThread = new QThread(this);    // рабочий поток (parent = this → удалится с окном)
    __database->moveToThread(__dbThread); //connectToDatabase() не заблокирует UI.
    connect(this,&MainWindow::sig_connectToDB, __database,  &Database::connectToDatabase);
    connect(__database, &Database::sig_connected, this,       &MainWindow::onConnected);
    connect(__database, &Database::sig_connectError, this,       &MainWindow::onConnectError);
    connect(__database, &Database::sig_disconnected,this,       &MainWindow::onDisconnected);
    connect(this,&MainWindow::sig_requestAirports, __database,  &Database::requestAirports);
    connect(__database, &Database::sig_airportsReady,this,       &MainWindow::onAirportsReady);

    connect(__dbThread, &QThread::finished,  __database, &QObject::deleteLater);    // Когда поток завершится (finished), удаляем Database.     // Это важно: __database создан без parent, и без этой связки он утечёт в память.
     __dbThread->start();     // start() запускает event loop в рабочем потоке. После этого __database может принимать сигналы.
    emit sig_connectToDB();  ///Database::connectToDatabase() вызовется в рабочем потоке. Подключение к БД произойдёт асинхронно.
}

// ═══════════════════════════════════════════════════════════════
// Деструктор — освобождаем память от объекта формы
// ═══════════════════════════════════════════════════════════════
MainWindow::~MainWindow()
{
    __dbThread->quit();     // quit() — просит event loop рабочего потока завершиться.
    __dbThread->wait();     // wait() — ждёт, пока поток реально завершится.
    delete ui;
}

void MainWindow::onConnected() // вызывается при успешном подключении к БД Обновляет статус (ФТ3
{
    ui->lb_status->setText("Статус БД: Подключено");
    ui->lb_status->setStyleSheet("color: green; font-weight: bold;");
    emit sig_requestAirports(); //  Запрашиваем список аэропортов (ФТ7)     // Сигнал уйдёт в рабочий поток → Database::requestAirports()
}

void MainWindow::onConnectError(QString m_error) //вызывается при ошибке подключения (ФТ5)Показывает QMessageBox с текстом ошибки.
{
    // Обновляем статус
    ui->lb_status->setText("Статус БД: Отключено");
    ui->lb_status->setStyleSheet("color: red; font-weight: bold;");
    // После закрытия этого окна Database сам запустит таймер повторной попытки через 5 сек (ФТ6)
    QMessageBox::critical(this,     // Показываем окно с ошибкой (ФТ5)
                          "Ошибка подключения к БД",
                          "Не удалось подключиться к базе данных.\n\n"
                          "Ошибка: " + m_error + "\n\n"
                                          "Повторная попытка через 5 секунд...");
}

void MainWindow::onDisconnected()
{
    ui->lb_status->setText("Статус БД: Отключено");
    ui->lb_status->setStyleSheet("color: red; font-weight: bold;");
    lockUI();
}

void MainWindow::onAirportsReady(QList<QPair<QString, QString>> airports)
{
    ui->cb_airoport->clear(); // Очищаем список перед заполнением
    for (const auto &pair : airports) {    // Заполняем QComboBox
        ui->cb_airoport->addItem(pair.first, pair.second);         // addItem(текст_для_отображения, скрытые_данные) pair.first  = "Шереметьево"  → пользователь видит это  second используем в SQL-запросах
    }
    unlockUI(); // разблокируем интерфейс (ФТ4)
}

void MainWindow::lockUI() //блокирует кнопки и элементы выбора (ФТ4)
{
    ui->pb_search->setEnabled(false);
    ui->pb_statistic->setEnabled(false);
    ui->cb_airoport->setEnabled(false);
    ui->de_time->setEnabled(false);
    ui->groupBox_2->setEnabled(false);   // Группа "Направление" целиком
}

void MainWindow::unlockUI() // Вызывается на этапе 3, когда список аэропортов получен из БД.
{
    ui->pb_search->setEnabled(true);
    ui->pb_statistic->setEnabled(true);
    ui->cb_airoport->setEnabled(true);
    ui->de_time->setEnabled(true);
    ui->groupBox_2->setEnabled(true);
}
