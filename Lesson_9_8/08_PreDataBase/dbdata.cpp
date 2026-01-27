#include "dbdata.h"
#include "database.h"
#include "ui_dbdata.h"

DbData::DbData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DbData)
{
    ui->setupUi(this);

   this->setWindowTitle("Параметры подключения к БД");
    //Ресайзим вектор значений, по количеству полей необходимых для
    //подключения к БД
    __data.resize(NUM_DATA_FOR_CONNECT_TO_DB);
   ui->le_host->setText("981757-ca08998.tmweb.ru");
   ui->le_dbName->setText("netology_cpp");
   ui->le_login->setText("netology_usr_cpp");
   ui->le_pass->setText("CppNeto3");
   ui->spB_port->setValue(5432);

}

DbData::~DbData()
{
    delete ui;
}
/*!
 * \brief Обработчик кнопки "Ок"
 */
void DbData::on_buttonBox_accepted() // Обработчик нажатия кнопки "OK"
{

    //Добавляем данные в контейнер и передаем в главное окно
    __data[hostName] = ui->le_host->text();
    __data[dbName] = ui->le_dbName->text();
    __data[login] = ui->le_login->text();
    __data[pass] = ui->le_pass->text();
    __data[port] = ui->spB_port->text();

    emit sig_sendData(__data);

}

