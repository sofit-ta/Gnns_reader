#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "satellite_window.h"
#include "read.h"
#include <QTimer>
#include <QFileDialog>
#include "qcustomplot.h"
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


    objectSatPlot = new QCustomPlot(ui->objectLocation);
    objectSatPlot->setParent(ui->objectLocation); // Связываем с виджетом
    objectSatPlot->resize(ui->objectLocation->size()); // Устанавливаем размер

    // Пример: Настройка осей и начального графика
    objectSatPlot->xAxis->setLabel("X");
    objectSatPlot->yAxis->setLabel("Y");
    double xMin = -100;  // Например, минимальное значение по оси X
    double xMax = 100;   // Максимальное значение по оси X
    double yMin = -100;  // Минимальное значение по оси Y
    double yMax = 100;   // Максимальное значение по оси Y

    objectSatPlot->xAxis->setRange(xMin, xMax); // Устанавливаем диапазон для оси X
    objectSatPlot->yAxis->setRange(yMin, yMax); // Устанавливаем диапазон для оси Y
    objectSatPlot->xAxis->grid()->setSubGridVisible(true);  // Включаем под-сетки
    objectSatPlot->yAxis->grid()->setSubGridVisible(true);  // Включаем под-сетки

    objectSatPlot->replot();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete re;
}

QColor MainWindow::chooseSatColor(Sputnik satellite){
    if(satellite.PDOP==INFINITY){
        return QColor(qRgb(194,184,180));
    }
    return QColor(qRgb(142,242,141));
}

void MainWindow::plotSatelliteData() {
    const Sputniks& sats = re->satellites;
    objectSatPlot->clearPlottables(); // Очищаем старые данные
    objectSatPlot->clearItems(); // Очищаем старые элементы (текст и эллипсы)

    QPair<double, double> objectPos = {0.0, 0.0}; // Точка объекта

    // Добавляем точку объекта
    QCPGraph* objectPoint = objectSatPlot->addGraph();
    objectPoint->setLineStyle(QCPGraph::lsNone);
    objectPoint->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::blue, 10));
    objectPoint->addData(objectPos.first, objectPos.second);

    // Добавляем точки спутников и их номера
    // QCPGraph* satellitePoints = objectSatPlot->addGraph();
    // satellitePoints->setLineStyle(QCPGraph::lsNone);
    // QCPScatterStyle satelliteStyle;
    // satelliteStyle.setShape(QCPScatterStyle::ssDisc);  // Пустой центр
    // satelliteStyle.setPen(QPen(Qt::green, 2));           // Ободок зеленого цвета, толщиной 2px
    // satelliteStyle.setSize(20);                          // Увеличиваем размер точек
    // satellitePoints->setScatterStyle(satelliteStyle);
    // satellitePoints->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::gray, 20));
    double xMin = 0, xMax = 0, yMin = 0, yMax = 0;

    for (int id = 1; id < 93; id++) {
        if(sats.tab[id].Status){
            // Добавляем данные спутников
            //satellitePoints->addData(sats.tab[id].SatX,sats.tab[id].SatY);
            QColor textcolor = chooseSatColor(sats.tab[id]);
            // Добавляем метку с номером спутника
            QCPItemText* textLabel = new QCPItemText(objectSatPlot);
            textLabel->setPositionAlignment(Qt::AlignCenter);  // Выравнивание текста по центру
            textLabel->position->setCoords(sats.tab[id].SatX,sats.tab[id].SatY);  // Позиция метки (координаты спутника)
            if(id<10){
                textLabel->setText("0"+QString::number(id));
            }else{
            textLabel->setText(QString::number(id));  // Текст метки (номер спутника)
            }
            textLabel->setFont(QFont("Arial", 14));  // Шрифт для текста
            textLabel->setColor(Qt::black);  // Цвет текста
            textLabel->setBrush(QBrush(textcolor));  //фон
            textLabel->setPadding(QMargins(3, 3, 3, 3));  // Отступы вокруг текста (слева, сверху, справа, снизу)
            xMin = qMin(xMin, sats.tab[id].SatX);
            xMax = qMax(xMax, sats.tab[id].SatX);
            yMin = qMin(yMin, sats.tab[id].SatY);
            yMax = qMax(yMax, sats.tab[id].SatY);
        }
    }

    // Устанавливаем диапазоны осей
    double margin = 0.1; // 10% от максимального расстояния
    double xRange = qMax(fabs(xMax), fabs(xMin));
    double yRange = qMax(fabs(yMax), fabs(yMin));

    objectSatPlot->xAxis->setRange(-xRange * (1 + margin), xRange * (1 + margin));
    objectSatPlot->yAxis->setRange(-yRange * (1 + margin), yRange * (1 + margin));

    objectSatPlot->xAxis->grid()->setSubGridVisible(true); // Включаем под-сетки
    objectSatPlot->yAxis->grid()->setSubGridVisible(true); // Включаем под-сетки

    objectSatPlot->replot(); // Обновляем график
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
    //qDebug()<<key<<last_update_time.msecsTo(curr_time);
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
    if (!first_time){
        qDebug() << "graph заполнен";
        plotSatelliteData();
    }

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

