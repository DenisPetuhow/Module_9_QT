#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    sig = new Signals(this); // this передали родителя mainwindow следов при удалении и этот объект удалитьбся
    ui->rb_sendsig->setChecked(true);
    ui->le_str->setEnabled(false);
    ui->spB_num->setEnabled(false);

    connect(ui->rb_sendstr, &QRadioButton::clicked, this, [this] {
        ui->le_str->setEnabled(true);
        ui->spB_num->setEnabled(false);
    });

    connect(ui->rb_sendstr, &QRadioButton::clicked, this, [this] {
        ui->le_str->setEnabled(true);
        ui->spB_num->setEnabled(false);
    });



    connect(sig, &Signals::sig_SendSimleSignal, this, &MainWindow::ReceiveSignal, Qt::AutoConnection); // если коннетк продублировать при нажатии кнопки то происходит наложение действий в два ража каждый раз
    // чтобы срабатывало только один раз за клик надо добавить флаг
    //connect(sig, &Signals::sig_SendSimleSignal, this, &MainWindow::ReceiveSignal, static_cast<Qt::ConnectionType>(Qt::AutoConnection | Qt::UniqueConnection));
    //connect(sig, &Signals::sig_SendSimleSignal, this, [this] {ui->tb_result->append("принят сигнал");}); // через лямду


    // enum ConnectionType {
    //     AutoConnection, //
    //     DirectConnection, // если один поток то сразо управление передается слоту
    //     QueuedConnection, // когда многопоточность действие становится в очередь
    //     BlockingQueuedConnection, // заблокируется поток где вызван сигнал пока слот не выполнитсмя
    //     UniqueConnection =  0x80, // флаг указывающий что соединение только один раз
    //     SingleShotConnection = 0x100,
    // };


    connect(sig, &Signals::sig_SendString, this, &MainWindow::ReceiveSignalString, Qt::AutoConnection);
    connect(sig, &Signals::sig_SendNum, this, &MainWindow::ReceiveSignalNum, Qt::AutoConnection);
    //connect(sig, qOverload<QString>(&Signals::sig_SendString), this, qOverload<QString>(&MainWindow::ReceiveSignalString), Qt::AutoConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
    //delete sig;
}

void MainWindow::on_pb_sendSignal_clicked()
{
    if(ui->rb_sendsig->isChecked()) {
        sig->SendSignal();
    } else if (ui->rb_sendstr->isChecked())
         {
        emit sig->sig_SendString(ui->le_str->text());
    } else if (ui->rb_sendNum->isChecked())
    {
        emit sig->sig_SendNum(ui->spB_num->value());
    }


}

void MainWindow::ReceiveSignal()
{
    ui->tb_result->append("принят сигнал");
}

void MainWindow::ReceiveSignalString(QString str)
{
    ui->tb_result->append(QString("Добавлена строкаб '%1'").arg(str));
}

void MainWindow::ReceiveSignalNum(int num)
{
    ui->tb_result->append(QString("Добавлена строкаб '%1'").arg(num));
}


