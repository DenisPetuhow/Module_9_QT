#ifndef DIALOG_ANWER_H
#define DIALOG_ANWER_H

#include <QMainWindow>

namespace Ui {
class Dialog_anwer;
}

class Dialog_anwer : public QMainWindow
{
    Q_OBJECT

public:
    explicit Dialog_anwer(QWidget *parent = nullptr);
    ~Dialog_anwer();

signals:
    // Сигнал, который мы будем отправлять при успешном входе
    void loginSuccessful();


private slots:
    void on_pushButton_clicked();

private:
    Ui::Dialog_anwer *ui;
};

#endif // DIALOG_ANWER_H
