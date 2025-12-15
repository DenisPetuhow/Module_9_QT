#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialog_anwer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    // Слот для обработки успешного входа из диалога
    void onLoginSuccesFromDialog();

private:
    Ui::MainWindow *ui;
    Dialog_anwer* answer;
};
#endif // MAINWINDOW_H
