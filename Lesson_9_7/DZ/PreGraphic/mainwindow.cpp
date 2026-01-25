#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "graphwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pb_clearResult->setCheckable(true);

    // Подключаем сигнал к слоту (QueuedConnection для межпоточной передачи)
    connect(this, &MainWindow::dataReady,
            this, &MainWindow::onDataReady,
            Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/****************************************************/
/*!
@brief: Считывает данные из файла
@param: path - путь к файлу
        numberChannel - номер канала АЦП
*/
/****************************************************/
QVector<uint32_t> MainWindow::ReadFile(QString path, uint8_t numberChannel)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);

    if(file.isOpen() == false){
        if(pathToFile.isEmpty()){
            QMessageBox mb;
            mb.setWindowTitle("Ошибка");
            mb.setText("Ошибка открытия файла");
            mb.exec();
        }
    }

    QDataStream dataStream;
    dataStream.setDevice(&file);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    QVector<uint32_t> readData;
    readData.clear();
    uint32_t currentWorld = 0, sizeFrame = 0;

    // Парсинг бинарного файла
    while(dataStream.atEnd() == false){
        dataStream >> currentWorld;

        if(currentWorld == 0xFFFFFFFF){
            dataStream >> currentWorld;

            if(currentWorld < 0x80000000){
                dataStream >> sizeFrame;

                if(sizeFrame > 1500){
                    continue;
                }

                for(int i = 0; i < sizeFrame/sizeof(uint32_t); i++){
                    dataStream >> currentWorld;

                    if((currentWorld >> 24) == numberChannel){
                        readData.append(currentWorld);
                    }
                }
            }
        }
    }

    ui->chB_readSucces->setChecked(true);
    return readData;
}

/****************************************************/
/*!
@brief: Преобразование сырых данных в вольты
*/
/****************************************************/
QVector<double> MainWindow::ProcessFile(const QVector<uint32_t> dataFile)
{
    QVector<double> resultData;
    resultData.clear();

    foreach (int word, dataFile) {
        word &= 0x00FFFFFF;
        if(word > 0x800000){
            word -= 0x1000000;
        }
        double res = ((double)word / 6000000) * 10;
        resultData.append(res);
    }

    ui->chB_procFileSucces->setChecked(true);
    return resultData;
}

/****************************************************/
/*!
@brief: Поиск двух максимумов
*/
/****************************************************/
QVector<double> MainWindow::FindMax(QVector<double> resultData)
{
    double max = 0, sMax = 0;

    foreach (double num, resultData){
        if(num > max){
            max = num;
        }
    }

    foreach (double num, resultData){
        if(num > sMax && (qFuzzyCompare(num, max) == false)){
            sMax = num;
        }
    }

    QVector<double> maxs = {max, sMax};
    ui->chB_maxSucess->setChecked(true);
    return maxs;
}

/****************************************************/
/*!
@brief: Поиск двух минимумов
*/
/****************************************************/
QVector<double> MainWindow::FindMin(QVector<double> resultData)
{
    double min = 0, sMin = 0;
    QThread::sleep(1);

    foreach (double num, resultData){
        if(num < min){
            min = num;
        }
    }

    QThread::sleep(1);

    foreach (double num, resultData){
        if(num < sMin && (qFuzzyCompare(num, min) == false)){
            sMin = num;
        }
    }

    QVector<double> mins = {min, sMin};
    ui->chB_minSucess->setChecked(true);
    return mins;
}

/****************************************************/
/*!
@brief: Вывод результатов
*/
/****************************************************/
void MainWindow::DisplayResult(QVector<double> mins, QVector<double> maxs)
{
    ui->te_Result->append("Расчет закончен!");
    ui->te_Result->append("Первый минимум: " + QString::number(mins.first()));
    ui->te_Result->append("Второй минимум: " + QString::number(mins.at(1)));
    ui->te_Result->append("Первый максимум: " + QString::number(maxs.first()));
    ui->te_Result->append("Второй максимум: " + QString::number(maxs.at(1)));
}

/****************************************************/
/*!
@brief: ТРЕБОВАНИЕ 4 - Слот отображения графика в НОВОМ ОКНЕ
*/
/****************************************************/
void MainWindow::onDataReady(QVector<double> xData, QVector<double> yData)
{
    // Закрываем предыдущее окно если есть
    if(graphWindow){
        graphWindow->close();
        graphWindow = nullptr;
    }

    // ТРЕБОВАНИЕ 5: Создаём НОВОЕ ОКНО для графика
    graphWindow = new GraphWindow(xData, yData, this);
    graphWindow->setAttribute(Qt::WA_DeleteOnClose);

    connect(graphWindow, &QObject::destroyed, this, [this](){
        graphWindow = nullptr;
    });

    // Показываем новое окно с графиком
    graphWindow->show();
}

/****************************************************/
/*!
@brief: Обработчик кнопки "Выбрать путь"
*/
/****************************************************/
void MainWindow::on_pb_path_clicked()
{
    pathToFile = "";
    ui->le_path->clear();

    pathToFile = QFileDialog::getOpenFileName(this,
                                              tr("Открыть файл"), "/home/", tr("ADC Files (*.adc)"));

    ui->le_path->setText(pathToFile);
}

/****************************************************/
/*!
@brief: Обработчик кнопки "Старт"
*/
/****************************************************/
void MainWindow::on_pb_start_clicked()
{
    if(pathToFile.isEmpty()){
        QMessageBox mb;
        mb.setWindowTitle("Ошибка");
        mb.setText("Выберите файл!");
        mb.exec();
        return;
    }

    // Сброс чекбоксов
    ui->chB_maxSucess->setChecked(false);
    ui->chB_procFileSucces->setChecked(false);
    ui->chB_readSucces->setChecked(false);
    ui->chB_minSucess->setChecked(false);

    // Выбор канала
    int selectIndex = ui->cmB_numCh->currentIndex();
    if(selectIndex == 0){
        numberSelectChannel = 0xEA;
    }
    else if(selectIndex == 1){
        numberSelectChannel = 0xEF;
    }
    else if(selectIndex == 2){
        numberSelectChannel = 0xED;
    }

    // Лямбда чтения
    auto read = [&]{
        return ReadFile(pathToFile, numberSelectChannel);
    };

    // Лямбда обработки
    auto process = [&](QVector<uint32_t> res){
        return ProcessFile(res);
    };

    // ТРЕБОВАНИЕ 2: Лямбда findmax с наполнением данных для графика
    // ТРЕБОВАНИЕ 3: Сигнал внутри findmax
    auto findMax = [&](QVector<double> res){
        // Поиск экстремумов
        maxs = FindMax(res);
        mins = FindMin(res);
        DisplayResult(mins, maxs);

        // ========================================
        // ТРЕБОВАНИЕ 1: Первая секунда данных
        // ТРЕБОВАНИЕ 2: Наполнение данных в findmax
        // ========================================
        int pointsToShow = qMin(res.size(), ONE_SECOND);

        QVector<double> xData;
        QVector<double> yData;
        xData.resize(pointsToShow);
        yData.resize(pointsToShow);

        for(int i = 0; i < pointsToShow; ++i){
            xData[i] = static_cast<double>(i);  // Время в мс
            yData[i] = res[i];                   // Значение в В
        }

        // ========================================
        // ТРЕБОВАНИЕ 3: Сигнал о готовности данных
        // ========================================
        emit dataReady(xData, yData);
    };

    // Запуск цепочки в фоновом потоке
    auto result = QtConcurrent::run(read)
                      .then(process)
                      .then(findMax);
}

/****************************************************/
/*!
@brief: Обработчик кнопки "Очистить результат"
*/
/****************************************************/
void MainWindow::on_pb_clearResult_clicked()
{
    ui->te_Result->clear();
    ui->chB_maxSucess->setChecked(false);
    ui->chB_procFileSucces->setChecked(false);
    ui->chB_readSucces->setChecked(false);
    ui->chB_minSucess->setChecked(false);
}










