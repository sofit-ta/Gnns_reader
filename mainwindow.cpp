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
        "Временная метка", "Широта, N", "Долгота, E",
        "Высота", "Значение высоты", "Скорость по северу", "Скорость по востоку", "Скорость по вертикали",
        "Погрешность по северу", "Погрешность по северо-востоку", "Погрешность по вертикали", "Погрешность по востоку",
        "Погрешность по вертикали", "Погрешность по вертикали", "Вариация по северу", "Вариация по северо-востоку",
        "Вариация по вертикали", "Вариация по востоку", "Вариация по вертикали", "Вариация по вертикали"
    };

    QStringList list_results;
    list_results.fill("0", list_names.length());

    // Модель для таблицы (создаем таблицу с нужным количеством столбцов)
    model2 = new QStandardItemModel(list_names.length(), 2, this);
    model2->setHorizontalHeaderLabels({"Parameter", "Value"});

    // Инициализация таблицы
    fill_the_table();

    // Инициализация таймера
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTable);

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

    // Запускаем таймер с интервалом 10 секунд
    timer->start(5000); // 10000 миллисекунд = 10 секунд

}
void MainWindow::updateTable() {
    if (!button_is_pressed) {
        // Читаем данные из файла
        re->file_reading();
    } else {
        timer->stop(); // Остановка таймера, если кнопка была нажата
    }
}

// Метод для заполнения таблицы данными
void MainWindow::fill_the_table() {
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

    list_results << QString::number(parced_data.Timestamp)
                 << QString::number(parced_data.Lat)
                 << QString::number(parced_data.Long)
                 << QString::number(parced_data.Alt)
                 << parced_data.AltVal
                 << QString::number(parced_data.VelN)
                 << QString::number(parced_data.VelE)
                 << QString::number(parced_data.VelV)
                 << QString::number(parced_data.PCovN)
                 << QString::number(parced_data.PCovNE)
                 << QString::number(parced_data.PCovNV)
                 << QString::number(parced_data.PCovE)
                 << QString::number(parced_data.PCovEV)
                 << QString::number(parced_data.PCovV)
                 << QString::number(parced_data.VCovN)
                 << QString::number(parced_data.VCovNE)
                 << QString::number(parced_data.VCovNV)
                 << QString::number(parced_data.VCovE)
                 << QString::number(parced_data.VCovEV)
                 << QString::number(parced_data.VCovV);
    //qDebug()<<"list_results"<<list_results;

    // Заполняем каждую строку таблицы
    for (int row = 0; row < list_names.length(); ++row) {
        QStandardItem *constantItem = new QStandardItem(list_names.at(row));
        constantItem->setFlags(Qt::ItemIsEnabled); // Только для чтения
        model2->setItem(row, 0, constantItem);

        QStandardItem *resultItem = new QStandardItem(list_results.at(row));
        resultItem->setFlags(Qt::ItemIsEnabled); // Только для чтения
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
    timer->stop();            // Остановка таймера
}


void MainWindow::on_open_file_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Выберете файл для парсинга"), "/Users/sofia/", tr("Text files (*.txt)"));
    re->updateParcingFile(fileName);
    ui->statusbar->showMessage(QString("Файл для парсинга выбран"),1000);
}

