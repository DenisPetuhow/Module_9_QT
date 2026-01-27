#ifndef DBDATA_H
#define DBDATA_H

#include <QDialog>

namespace Ui {
class DbData;
}

class DbData : public QDialog // диалог ввода параметров подключения к БД После нажатия "OK" передаёт данные через сигнал sig_sendData.
{
    Q_OBJECT

public:
    explicit DbData(QWidget *parent = nullptr);
    ~DbData();


 signals:
    void sig_sendData(QVector<QString> dbData); // Сигнал передачи данных подключения


private slots:
    void on_buttonBox_accepted();     // Обработчик нажатия кнопки "OK"

private:
    Ui::DbData *ui;
    QVector<QString> __data;  // Контейнер для данных подключения

};

#endif // DBDATA_H
