#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "satellite_window.h"
#include "read.h"
#include <QTimer>
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , re(new read(this, this))  // Передаем указатель на MainWindow в read
    , model2(new QStandardItemModel(this))
{
    ui->setupUi(this);

    // Инициализация списка названий столбцов
    list_names = {
        "Широта, N", "Долгота, E",
        "Высота", "Скорость, км/ч", "Направление, градусы"
    };

    QStringList list_results;
    list_results.fill("0", list_names.length());

    // Модель для таблицы (создаем таблицу с нужным количеством столбцов)
    model2 = new QStandardItemModel(list_names.length(), 2, this);
    model2->setHorizontalHeaderLabels({"Parameter", "Value"});

    // Инициализация таблицы
    fill_the_table(true);

    // Инициализация таймера
    //timer = new QTimer(this);
    //connect(timer, &QTimer::timeout, this, &MainWindow::updateTable);

    model = new QStringListModel(this);
    QStringList list_of_s1 = {"32","11","14","01"};
    model->setStringList(list_of_s1);
    ui->listView->setModel(model);

    model1 = new QStringListModel(this);
    QStringList list_of_s2 = {"15","32","02","03"};
    model1->setStringList(list_of_s2);
    ui->listView_2->setModel(model1);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::startReadingData);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::stopReadingData);
    connect(ui->open_file, &QAction::triggered, this, &MainWindow::on_open_file_triggered);

    clocks = new QTimer(this);
    connect(clocks, &QTimer::timeout, this, &MainWindow::updateClock);


}

MainWindow::~MainWindow()
{
    delete ui;
    delete re;
}



void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    QString selectedText = ui->listView->model()->data(index, Qt::DisplayRole).toString();
    ui->statusbar->showMessage("Выбран спутник "+ selectedText);
}

void MainWindow::updateClock() {
    re->clock_time = re->clock_time.addSecs(1);
    qDebug()<<re->clock_time.toString("hh:mm:ss");
    QString currentTime = re->clock_time.toString("hh:mm:ss");
    ui->clock->setText(currentTime);
    updateTable();
}

void MainWindow::on_listView_2_clicked(const QModelIndex &index)
{
    QString selectedText = ui->listView_2->model()->data(index, Qt::DisplayRole).toString();
    ui->statusbar->showMessage("Выбран спутник "+ selectedText);
}


void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
    QString selectedData = ui->listView->model()->data(index, Qt::DisplayRole).toString();
    sw = new Satellite_window(selectedData,this);
    sw -> show();
}


void MainWindow::on_listView_2_doubleClicked(const QModelIndex &index)
{
    QString selectedData = ui->listView_2->model()->data(index, Qt::DisplayRole).toString();
    sw = new Satellite_window(selectedData,this);
    sw -> show();
}

bool MainWindow::button_is_pressed = false;

void MainWindow::startReadingData()
{
    qDebug() << "start";
    button_is_pressed = false; // Сброс состояния
    // Запускаем таймер
    clocks->start(1000); // обновляется каждую секунду

    // Обновляем сразу же, чтобы пользователь увидел начальное значение
    //updateClock();
    // Запускаем таймер с интервалом 10 секунд
    //timer->start(5000); // 10000 миллисекунд = 10 секунд

}
void MainWindow::updateTable() {
    if (!button_is_pressed) {
        // Читаем данные из файла
        re->file_reading();
    }
    else {
         clocks->stop(); // Остановка таймера, если кнопка была нажата
    }
}
QColor MainWindow::updateColor(const QString key,ResultStructure parced_data){
    QTime curr_time = re->clock_time;
    QTime last_update_time = parced_data.getUpdateTime(key.toStdString());
    qDebug()<<key<<last_update_time.msecsTo(curr_time);
    if (last_update_time.msecsTo(curr_time) <= 5000) {
        return Qt::green;
    } else if (last_update_time.msecsTo(curr_time) <= 10000){
        return Qt::yellow;
    } else {
        return Qt::red;
    }
}
// QString MainWindow::last_value(QVector<double>) {
//     // Добавьте вашу логику обработки данных SoGK здесь.
//     // Например, округлим значение до двух знаков после запятой.
//     return QString::number();
// }
// Метод для заполнения таблицы данными
void MainWindow::fill_the_table(bool first_time) {
    if (re == nullptr) {
        qDebug() << "Ошибка: объект re не инициализирован.";
        return;
    }

    // Очищаем таблицу перед заполнением новыми данными
    model2->clear();

    // Заполняем таблицу всеми записями из вектора data_list

    const ResultStructure& parced_data = re->data_info;
    QStringList list_results;
    if (parced_data.Timestamp == -5) {
        qDebug() << "Нет данных для отображения.";
        return;
    }
    QStringList keys = {
        "Lat", "Long",
        "Alt", "SoGN", "TMGT"
    };
    list_results
                 << QString::number(parced_data.Lat)
                 << QString::number(parced_data.Long)
                 << QString::number(parced_data.Alt)
                 << QString::number(parced_data.SoGN)
        << QString::number(parced_data.TMGT);
    //qDebug()<<"list_results"<<list_results;

    // Заполняем каждую строку таблицы
    for (int row = 0; row < list_names.length(); ++row) {
        QStandardItem *constantItem = new QStandardItem(list_names.at(row));
        constantItem->setFlags(Qt::ItemIsEnabled); // Только для чтения
        model2->setItem(row, 0, constantItem);

        QStandardItem *resultItem = new QStandardItem(list_results.at(row));
        resultItem->setFlags(Qt::ItemIsEnabled); // Только для чтения
        if (!first_time){
            resultItem->setBackground(updateColor(keys.at(row),parced_data));
        }
        model2->setItem(row, 1, resultItem);
    }

    // Устанавливаем модель для TableView
    ui->tableView->setModel(model2);

    // Подгоняем размер столбцов под текст
    ui->tableView->resizeColumnsToContents();
    qDebug() << "Таблица заполнена";
}

void MainWindow::stopReadingData()
{
    qDebug() << "stop";
    button_is_pressed = true; // Установка флага
    clocks->stop();
    //timer->stop();            // Остановка таймера
}


void MainWindow::on_open_file_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Выберете файл для парсинга"), "/Users/sofia/", tr("Text files (*.txt)"));
    re->updateParcingFile(fileName);
    ui->statusbar->showMessage(QString("Файл для парсинга выбран"),1000);
}

