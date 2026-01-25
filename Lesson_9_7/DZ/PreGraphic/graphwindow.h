#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H

#include <QWidget>
#include "qcustomplot.h"

/*!
 * @class GraphWindow
 * @brief ТРЕБОВАНИЕ 5: Новое окно для отображения графика
 */
class GraphWindow : public QWidget
{
    Q_OBJECT

public:
    explicit GraphWindow(const QVector<double>& xData,
                         const QVector<double>& yData,
                         QWidget *parent = nullptr);
    ~GraphWindow();

private:
    QCustomPlot* customPlot;  // Виджет графика
    QCPGraph* graph;          // Серия данных

    void setupPlot();
};

#endif // GRAPHWINDOW_H










