#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QFileDialog>
#include <QDataStream>
#include <QMessageBox>
#include <QThread>
#include <QtConcurrent>

// Предварительное объявление окна графика
class GraphWindow;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    // ТРЕБОВАНИЕ 3: Сигнал о готовности данных для отрисовки
    // Передаём два вектора: X (время) и Y (значения)
    void dataReady(QVector<double> xData, QVector<double> yData);

private slots:
    void on_pb_path_clicked();
    void on_pb_start_clicked();
    void on_pb_clearResult_clicked();

    // ТРЕБОВАНИЕ 4: Слот отображения графика
    void onDataReady(QVector<double> xData, QVector<double> yData);

private:
    Ui::MainWindow *ui;

    QString pathToFile;
    uint8_t numberSelectChannel;
    QVector<double> maxs;
    QVector<double> mins;

    // ТРЕБОВАНИЕ 5: Указатель на НОВОЕ ОКНО графика
    GraphWindow* graphWindow = nullptr;

    // Нужно (добавить constexpr):
    static constexpr int SAMPLE_RATE = 1000;
    static constexpr int ONE_SECOND = 1000;

    // Методы обработки
    QVector<uint32_t> ReadFile(QString path, uint8_t numberChannel);
    QVector<double> ProcessFile(const QVector<uint32_t> dataFile);
    QVector<double> FindMax(QVector<double> resultData);
    QVector<double> FindMin(QVector<double> resultData);
    void DisplayResult(QVector<double> mins, QVector<double> maxs);
};

#endif // MAINWINDOW_H
