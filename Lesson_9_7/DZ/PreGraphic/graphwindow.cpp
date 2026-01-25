#include "graphwindow.h"
#include <QVBoxLayout>

/*!
 * @brief Конструктор нового окна графика
 * @param xData - данные по оси X (время в мс)
 * @param yData - данные по оси Y (напряжение в В)
 */
GraphWindow::GraphWindow(const QVector<double>& xData,
                         const QVector<double>& yData,
                         QWidget *parent)
    : QWidget(parent)
{
    // Настройки окна
    setWindowTitle("График первой секунды данных");
    setMinimumSize(800, 600);

    // Создаём виджет QCustomPlot
    customPlot = new QCustomPlot(this);

    // Создаём график (привязываем к осям)
    graph = new QCPGraph(customPlot->xAxis, customPlot->yAxis);

    // Добавляем данные на график
    graph->addData(xData, yData);

    // Настраиваем внешний вид
    setupPlot();

    // Размещаем в layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->addWidget(customPlot);
    setLayout(layout);
}

GraphWindow::~GraphWindow()
{
    // customPlot удалится автоматически как дочерний виджет
}

/*!
 * @brief Настройка внешнего вида графика
 */
void GraphWindow::setupPlot()
{
    // Заголовок
    customPlot->plotLayout()->insertRow(0);
    customPlot->plotLayout()->addElement(0, 0,
                                         new QCPTextElement(customPlot,
                                                            "Первая секунда обработанных данных (1000 Гц)",
                                                            QFont("sans", 12, QFont::Bold)));

    // Подписи осей
    customPlot->xAxis->setLabel("Время (мс)");
    customPlot->yAxis->setLabel("Напряжение (В)");

    // Стиль линии
    graph->setPen(QPen(Qt::blue, 1));
    graph->setName("Сигнал АЦП");

    // Легенда
    customPlot->legend->setVisible(true);

    // Автомасштабирование по данным
    customPlot->rescaleAxes();

    // Интерактивность
    customPlot->setInteraction(QCP::iRangeZoom, true);   // Зум колёсиком
    customPlot->setInteraction(QCP::iRangeDrag, true);   // Перетаскивание
    customPlot->setInteraction(QCP::iSelectPlottables, true);

    // Отрисовка
    customPlot->replot();
}
