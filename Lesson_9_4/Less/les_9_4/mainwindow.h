#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QListWidgetItem"

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
    void on_le_inputText_textChanged(const QString &arg1);

    void on_le_inputText_textEdited(const QString &arg1);

    void on_lw_list_itemDoubleClicked(QListWidgetItem *item);

    void on_pb_clear_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
