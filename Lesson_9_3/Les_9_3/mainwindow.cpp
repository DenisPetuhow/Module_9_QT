#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    answer = new Dialog_anwer(this);
    connect(answer, &Dialog_anwer::loginSuccessful, this, &MainWindow::onLoginSuccesFromDialog);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete answer;
}

void MainWindow::on_pushButton_clicked()
{
    answer -> show();
}

void MainWindow::onLoginSuccesFromDialog()
{

    ui->textEdit->setText("вы успешно вошли");
}
