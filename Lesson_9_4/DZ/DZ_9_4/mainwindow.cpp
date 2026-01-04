#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
     m_counter(0) // Изначально счетчик 0
{
    ui->setupUi(this);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::on_timer_tick);  // Соединяем сигнал таймера с нашей функцией on_timer_tick


    // --- 1. ПЕРЕИМЕНОВАНИЕ ВКЛАДОК ---
    ui->tabWidget->setTabText(0, "Основа");
    ui->tabWidget->setTabText(1, "Настройки");

    // --- 2. НАЗВАНИЯ ДЛЯ GROUPBOX ---
    ui->gb_rad->setTitle("Сценарий выполнения");
    ui->gb_progress->setTitle("Формат отображения");
    ui->gb_global->setTitle("Панель управления");

    // 1. Наполняем ComboBox (cb_bar) вариантами
    ui->cb_bar->addItem("Число");           // index 0
    ui->cb_bar->addItem("Проценты (%)");    // index 1
    ui->cb_bar->addItem("Дробь (тек/макс)");// index 2

    ui->rb_pr->setText("Вариант первый проценты ");
    ui->rb_day->setText("Вариант второй дни");

    // 2. Настраиваем начальное состояние RadioButton
    ui->rb_pr->setChecked(true);

    // 3. Настраиваем Прогресс-бар (pr_bar)
    ui->pr_bar->setValue(0);
    ui->pr_bar->setAlignment(Qt::AlignCenter); // Текст по центру

    // 4. Настраиваем Кнопку (bt_push)
    ui->bt_push->setText("Добавить +10%");

    // !!! ВАЖНО: Делаем кнопку "залипающей", иначе Start/Stop не сработает !!!
    ui->bt_push->setCheckable(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_rb_pr_toggled(bool checked)
{
    if (checked) {
        // Устанавливаем максимум 100
        ui->pr_bar->setRange(0, 100);

        // Сбрасываем счетчик
        m_counter = 0;
        ui->pr_bar->setValue(m_counter);
        ui->bt_push->setText("Добавить +10%");
        // Если работает таймер - сбрасываем (отжимаем кнопку)
        if (ui->bt_push->isChecked()) ui->bt_push->click();
    }
}
void MainWindow::on_rb_day_toggled(bool checked)
{
    if (checked) {
        // Устанавливаем максимум 365
        ui->pr_bar->setRange(0, 365);
        m_counter = 0;
        ui->pr_bar->setValue(m_counter);
        // Меняем подпись на кнопке
        ui->bt_push->setText("Добавить +1 день");
        // Если работает таймер - сбрасываем (отжимаем кнопку)
        if (ui->bt_push->isChecked()) ui->bt_push->click();
    }
}

void MainWindow::on_cb_bar_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        // Просто число (%v - value)
        ui->pr_bar->setFormat("%v");
        break;
    case 1:
        // Проценты (%p - percent)
        ui->pr_bar->setFormat("%p%");
        break;
    case 2:
        // Дробь (%v - value, %m - max)
        ui->pr_bar->setFormat("%v из %m");
        break;
    }
}


void MainWindow::on_bt_push_clicked(bool checked)
{
    // checked == true, если кнопка вдавилась (ВКЛ)
    // checked == false, если кнопка отжалась (ВЫКЛ)

    if (checked) {
        ui->bt_push->setText("Стоп"); // Меняем надпись
        m_timer->start(1000);         // Запускаем таймер (1000 мс = 1 сек)
    } else {
        ui->bt_push->setText("Старт");
        m_timer->stop();              // Останавливаем таймер
    }
}

void MainWindow::on_timer_tick()
{
    //Выбираем шаг добавления
    int step = 0;
    if (ui->rb_pr->isChecked()) {
        step = 10; // Если проценты -> +10
    } else {
        step = 1;  // Если дни -> +1
    }
    //Увеличиваем счетчик
    m_counter += step;
    // Проверка на конец полоски
    if (m_counter > ui->pr_bar->maximum()) {
        m_counter = 0; // Начинаем заново

    }
    //Обновляем прогресс-бар
    ui->pr_bar->setValue(m_counter);
}

