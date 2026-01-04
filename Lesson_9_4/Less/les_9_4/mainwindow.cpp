#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("HELLO");
    // Настройка поля ввода (le_inputText)
    ui->le_inputText->setEnabled(true);
    ui->le_inputText->setText("Привет!");
    ui->le_inputText->clear();


    // Настройка счетчика (spB_counter)
    ui->spb_counter->setMaximum(100);
    ui->spb_counter->setMinimum(0);
    ui->spb_counter->setSingleStep(1);
    ui->spb_counter->setValue(0);
    ui->spb_counter->setReadOnly(true);

    // Заполнение списка (lw_list)
    for(int i = 1; i <= 11; ++i){
        ui->lw_list->addItem(QString("Элемент # %1").arg(i));
    }

    ui->lw_list->takeItem(10); // удаление item по номеру строки

    // Настройка прогресс-бара (pBar_bar)
    // В скриншотах были разные варианты, но этот соответствует логике сброса на 20
    ui->pBar_bar->setMaximum(20);
    ui->pBar_bar->setMinimum(0);
    ui->pBar_bar->setValue(0);

    ui->pBar_bar->setFormat("%p%");

    //ui->gb_inputt->layout()->addwidget(); //добавление виджетов из кода
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_le_inputText_textChanged(const QString &arg1) // когда изменения из программы
{
    QString str = ui->le_inputText->text();
    qDebug() << "on_le_inputText_textChanged - " << str; // сожно так или сразу добавить arg
}


void MainWindow::on_le_inputText_textEdited(const QString &arg1) //когда изменения из виджета
{
    qDebug() << "on_le_inputText_textEdited - " << arg1;
    int val = ui->spb_counter->value();

    // Логика сброса при достижении 20
    if(val >= 20){
        ui->te_log->clear();
        val = 0;
    }

    ++val;
    if (ui->cb_couter->isChecked() == false) {
        ui->spb_counter->setValue(val);
    };

    if (ui->cb_bar->isChecked() == false) {
    ui->pBar_bar->setValue(val);
    };


    // Добавление записи в лог
    ui->te_log->append(QString("Был введен текст: '%1'").arg(arg1));
    QString str1 = ui->te_log->toPlainText(); // Возвращает неотформатированный текст из TextEdit
}


void MainWindow::on_lw_list_itemDoubleClicked(QListWidgetItem *item) // если делаем дваойной клик по таблице то вставляем значениен
{
    int val = ui->spb_counter->value();
    // Логика сброса при достижении 20
    if(val >= 20){
        ui->te_log->clear();
        val = 0;
    }

    ++val;
    ui->spb_counter->setValue(val);
    ui->pBar_bar->setValue(val);

    // Добавление записи в лог
    ui->te_log->append(QString("Был введен текст: '%1'").arg(item->text()));
}


void MainWindow::on_pb_clear_clicked()
{
    ui->te_log->clear();
    ui->spb_counter->setValue(0);
    ui->pBar_bar->setValue(0);
}

