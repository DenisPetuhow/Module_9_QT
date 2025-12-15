#include "dialog_anwer.h"
#include "ui_dialog_anwer.h"
#include <QMessageBox>

Dialog_anwer::Dialog_anwer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Dialog_anwer)
{
    ui->setupUi(this);
}

Dialog_anwer::~Dialog_anwer()
{
    delete ui;
}

void Dialog_anwer::on_pushButton_clicked()
{
    QString host = ui->le_host->text();
    QString dbName = ui->le_BD->text();
    QString login = ui->le_log->text();
    QString pass = ui->le_pas->text();

    // так как там всегда есть число. Но можно проверить, например, не 0 ли это.
    int port = ui->spB_port->value();

    // 2. Проверяем, пусты ли текстовые поля
    if (host.isEmpty() || dbName.isEmpty() || login.isEmpty() || pass.isEmpty()) {

        // Если хотя бы одно пустое -> показываем ошибку
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля!");

        // ВАЖНО: return прерывает функцию, окно НЕ закроется, сигнал НЕ отправится
        return;
    }
    emit loginSuccessful(); // Отправляем сигнал
    this->close();          // Закрываем окно
}

