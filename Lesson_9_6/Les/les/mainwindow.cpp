#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtConcurrent>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sb_initNum->setValue(200000);
    ui->rb_synchOff->setChecked(true);

    // === Создание контроллеров потоков ===
    // Передаем один и тот же мьютекс обоим контроллерам
    race1 = new Controller(&m);
    race2 = new Controller(&m);

        // === Создание объектов для QtConcurrent ===
    concurRace1 = new ExampleRace(&m);
    concurRace2 = new ExampleRace(&m);

    // Логика обработки завершения:
    // 1. Первый завершившийся поток инкрементирует countFinish (0->1)
    // 2. Второй завершившийся поток видит countFinish==1 и выводит результат
    //Сигналы по завершению работы потоков
    connect(race1, &Controller::sig_WorkFinish, [&](){
        //отображаем только когда второй поток закончит работу
        if(countFinish == 0){
            countFinish++;
        }
        else{
            ui->te_debug->append("Искомое число равно: " + QString::number(number) + ", а должно быть " +
                                 QString::number(ui->sb_initNum->value()*2));
            ui->pb_start->setEnabled(true);
        }

    });

    connect(race2, &Controller::sig_WorkFinish, [&](){

        if(countFinish == 0){
            countFinish++;
        }
        else{
            ui->te_debug->append("Искомое число равно: " + QString::number(number) + ", а должно быть " +
                                 QString::number(ui->sb_initNum->value()*2));
            ui->pb_start->setEnabled(true);
        }

    });


    connect(concurRace1, &ExampleRace::sig_Finish, [&](){

        if(countFinish == 0){
            countFinish++;
        }
        else{
            ui->te_debug->append("Искомое число равно: " + QString::number(number) + ", а должно быть " +
                                 QString::number(ui->sb_initNum->value()*2));
            ui->pb_start->setEnabled(true);
        }
    });

    connect(concurRace2, &ExampleRace::sig_Finish, [&](){

        if(countFinish == 0){
            countFinish++;
        }
        else{
            ui->te_debug->append("Искомое число равно: " + QString::number(number) + ", а должно быть " +
                                 QString::number(ui->sb_initNum->value()*2));
            ui->pb_start->setEnabled(true);
        }
    });
}

MainWindow::~MainWindow()
{
        // (Controller::~Controller() сам завершит потоки)
    delete ui;
    delete race1;
    delete race2;
    delete concurRace1;
    delete concurRace2;
}

 //Запускает два потока для демонстрации race condition
void MainWindow::StartRace(void){


    if(ui->rb_qtConcur->isChecked()){
         // QtConcurrent::run() - запускает функцию в отдельном потоке из thread pool озвращает QFuture
        QtConcurrent::run([this](){
            concurRace1->DoWork(&number,
                                ui->rb_mutexOn->isChecked(),
                                ui->sb_initNum->value());
        });

        QtConcurrent::run([this](){
            concurRace2->DoWork(&number,
                                ui->rb_mutexOn->isChecked(),
                                ui->sb_initNum->value());
        });

        // ПРИМЕЧАНИЕ: В QtConcurrent::run() лямбда выполняется в отдельном потоке,
        // но сигнал sig_Finish() будет испущен из этого потока.
        // Коннекты в конструкторе должны использовать Qt::QueuedConnection
        // для безопасной передачи между потоками.


        ui->te_debug->append("Выполни ДЗ!");
        //Тут должен быть код ДЗ

    }
    else{
        race1->operate(&number, ui->rb_mutexOn->isChecked(), ui->sb_initNum->value());
        race2->operate(&number, ui->rb_mutexOn->isChecked(), ui->sb_initNum->value());
    }
}

//Обработка кнопки "Старт"
void MainWindow::on_pb_start_clicked()
{
    ui->pb_start->setEnabled(false);
    countFinish = 0;
    number = 0;
    StartRace( );
}

void MainWindow::on_pb_clear_clicked()
{
    ui->te_debug->clear();
}

