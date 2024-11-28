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
    //model->setStringList(list_of_sats_GPS);
    ui->listView->setModel(model);

    model1 = new QStringListModel(this);
    //model1->setStringList(list_of_sats_GLONASS);
    ui->listView_2->setModel(model1);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::startReadingData);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::stopReadingData);
    connect(ui->open_file, &QAction::triggered, this, &MainWindow::on_open_file_triggered);

    clocks = new QTimer(this);
    connect(clocks, &QTimer::timeout, this, &MainWindow::updateClock);

    //график для окна положения объекта широта
    objectLatPlot = new QCustomPlot(ui->objectLocationLat);
    objectLatPlot->setParent(ui->objectLocationLat); // Связываем с виджетом
    objectLatPlot->resize(ui->objectLocationLat->size()); // Устанавливаем размер
    // Настройка осей
    objectLatPlot->addGraph();
    objectLatPlot->xAxis->setLabel("Время, с");
    objectLatPlot->yAxis->setLabel("Широта, градусы");
    objectLatPlot->xAxis->setRange(0, 60); // 60 секунд
    objectLatPlot->yAxis->setRange(-90, 90); // Диапазон широт

    // Добавляем график
    objectLatPlot->graph(0)->setPen(QPen(Qt::blue));

    //график для окна положения объекта долгота
    objectLongPlot = new QCustomPlot(ui->objectLocationLong);
    objectLongPlot->setParent(ui->objectLocationLong); // Связываем с виджетом
    objectLongPlot->resize(ui->objectLocationLong->size()); // Устанавливаем размер
    // Настройка осей
    objectLongPlot->addGraph();
    objectLongPlot->xAxis->setLabel("Время, с");
    objectLongPlot->yAxis->setLabel("Долгота, градусы");
    objectLongPlot->xAxis->setRange(0, 60); // 60 секунд
    objectLongPlot->yAxis->setRange(-90, 90); // Диапазон

    // Добавляем график
    objectLongPlot->graph(0)->setPen(QPen(Qt::blue));

    //график для окна показа спутников
    currSatPlot = new QCustomPlot(ui->currSats);
    currSatPlot->setParent(ui->currSats); // Связываем с виджетом
    currSatPlot->resize(ui->currSats->size()); // Устанавливаем размер
    set_the_plot(currSatPlot);

    //график для окна выбора спутника
    chosenSatPlot = new QCustomPlot(ui->chosenSat);
    chosenSatPlot->setParent(ui->chosenSat); // Связываем с виджетом
    chosenSatPlot->resize(ui->chosenSat->size()); // Устанавливаем размер
    set_the_plot(chosenSatPlot);

    //график для окна выбора спутника х
    satXPlot = new QCustomPlot(ui->sputnik_X);
    satXPlot->setParent(ui->sputnik_X); // Связываем с виджетом
    satXPlot->resize(ui->sputnik_X->size()); // Устанавливаем размер
    // Настройка осей
    satXPlot->addGraph();
    satXPlot->xAxis->setLabel("Время, с");
    satXPlot->yAxis->setLabel("Координата X, м");
    satXPlot->xAxis->setRange(0, 60); // 60 секунд
    satXPlot->yAxis->setRange(-90, 90); // Диапазон

    // Добавляем график
    satXPlot->graph(0)->setPen(QPen(Qt::blue));

    //график для окна выбора спутника y
    satYPlot = new QCustomPlot(ui->sputnik_Y);
    satYPlot->setParent(ui->sputnik_Y); // Связываем с виджетом
    satYPlot->resize(ui->sputnik_Y->size()); // Устанавливаем размер
    // Настройка осей
    satYPlot->addGraph();
    satYPlot->xAxis->setLabel("Время, с");
    satYPlot->yAxis->setLabel("Координата Y, м");
    satYPlot->xAxis->setRange(0, 60); // 60 секунд
    satYPlot->yAxis->setRange(-90, 90); // Диапазон

    // Добавляем график
    satYPlot->graph(0)->setPen(QPen(Qt::blue));
    //график для окна выбора спутника z
    satZPlot = new QCustomPlot(ui->sputnik_Z);
    satZPlot->setParent(ui->sputnik_Z); // Связываем с виджетом
    satZPlot->resize(ui->sputnik_Z->size()); // Устанавливаем размер
    // Настройка осей
    satZPlot->addGraph();
    satZPlot->xAxis->setLabel("Время, с");
    satZPlot->yAxis->setLabel("Координата Z, м");
    satZPlot->xAxis->setRange(0, 60); // 60 секунд
    satZPlot->yAxis->setRange(-90, 90); // Диапазон

    // Добавляем график
    satZPlot->graph(0)->setPen(QPen(Qt::blue));
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

void MainWindow::set_the_plot(QCustomPlot *plot){
    plot->xAxis->setLabel("Координата Х, м");
    plot->yAxis->setLabel("Координата Y, м");
    double xMin = -100;  // Например, минимальное значение по оси X
    double xMax = 100;   // Максимальное значение по оси X
    double yMin = -100;  // Минимальное значение по оси Y
    double yMax = 100;   // Максимальное значение по оси Y

    plot->xAxis->setRange(xMin, xMax); // Устанавливаем диапазон для оси X
    plot->yAxis->setRange(yMin, yMax); // Устанавливаем диапазон для оси Y
    plot->xAxis->grid()->setSubGridVisible(true);  // Включаем под-сетки
    plot->yAxis->grid()->setSubGridVisible(true);  // Включаем под-сетки

    plot->replot();
}

void MainWindow::plotSatelliteData() {
    const Sputniks& sats = re->satellites;
    currSatPlot->clearPlottables(); // Очищаем старые данные
    currSatPlot->clearItems(); // Очищаем старые элементы (текст и эллипсы)

    QPair<double, double> objectPos = {0.0, 0.0}; // Точка объекта

    // Добавляем точку объекта
    QCPGraph* objectPoint = currSatPlot->addGraph();
    objectPoint->setLineStyle(QCPGraph::lsNone);
    objectPoint->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::blue, 10));
    objectPoint->addData(objectPos.first, objectPos.second);

    double xMin = 0, xMax = 0, yMin = 0, yMax = 0;
    for (int id = 1; id < 93; id++) {
        if(sats.tab[id].Status){
            if(id<33&&id!=0&&!list_of_sats_GPS.contains(id)){
                list_of_sats_GPS.append(id);
                std::sort(list_of_sats_GPS.begin(), list_of_sats_GPS.end());
                int rowToAdd = list_of_sats_GPS.indexOf(id);
                model->insertRows(rowToAdd, 1);
                QModelIndex index = model->index(rowToAdd); // получаем индекс новой строки
                model->setData(index, QString::number(id)); // задаем значение для строки

            }else if(id>64&&id<93&&!list_of_sats_GLONASS.contains(id)){
                list_of_sats_GLONASS.append(id);
                std::sort(list_of_sats_GLONASS.begin(), list_of_sats_GLONASS.end());
                int rowToAdd = list_of_sats_GLONASS.indexOf(id);
                model1->insertRows(rowToAdd, 1);
                QModelIndex index = model1->index(rowToAdd); // получаем индекс новой строки
                model1->setData(index, QString::number(id)); // задаем значение для строки
            }
            // Добавляем данные спутников
            QColor textcolor = chooseSatColor(sats.tab[id]);
            // Добавляем метку с номером спутника
            QCPItemText* textLabel = new QCPItemText(currSatPlot);
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
        }else if(list_of_sats_GPS.contains(id)||list_of_sats_GLONASS.contains(id)){
            if(id<33){
                int index = list_of_sats_GPS.indexOf(id);
                model->removeRows(index, 1);
                list_of_sats_GPS.removeAt(index);
            }else{
                int index = list_of_sats_GLONASS.indexOf(id);
                model1->removeRows(index, 1);
                list_of_sats_GLONASS.removeAt(index);
            }
        }
    }
    // Устанавливаем диапазоны осей
    double margin = 0.1; // 10% от максимального расстояния
    xRange = qMax(fabs(xMax), fabs(xMin));
    yRange = qMax(fabs(yMax), fabs(yMin));

    currSatPlot->xAxis->setRange(-xRange * (1 + margin), xRange * (1 + margin));
    currSatPlot->yAxis->setRange(-yRange * (1 + margin), yRange * (1 + margin));

    currSatPlot->xAxis->grid()->setSubGridVisible(true); // Включаем под-сетки
    currSatPlot->yAxis->grid()->setSubGridVisible(true); // Включаем под-сетки

    //qDebug() << "graph 1 заполнен";
    currSatPlot->replot(); // Обновляем график
}




void MainWindow::plotObject_Data() {
    const ResultStructure& parced_data = re->data_info;
    // double currentTime = QTime(0,0,0,0).secsTo(timer_time);
    // ObjectTimeforplot.append(currentTime);
    // ObjectLat.append(parced_data.Lat);
    // ObjectLong.append(parced_data.Long);
    double currentTime = QTime(0,0,0,0).secsTo(timer_time);
    QList <double> ObjectLatTime = parced_data.Timing_history_Lat;
    QList <double> ObjectLat = parced_data.Lat_history;
    if(currentTime!=ObjectLatTime.last()){
        ObjectLatTime.append(currentTime);
        ObjectLat.append(ObjectLat.last());
    }
    QList <double> ObjectLongTime = parced_data.Timing_history_Long;
    QList <double> ObjectLong = parced_data.Long_history;
    if(currentTime!=ObjectLongTime.last()){
        ObjectLongTime.append(currentTime);
        ObjectLong.append(ObjectLong.last());
    }
    // Обновляем графики
    objectLatPlot->graph(0)->setData(ObjectLatTime, ObjectLat);
    objectLatPlot->xAxis->setRange(0, currentTime); // Диапазон по времени
    objectLatPlot->yAxis->setRange(*std::min_element(ObjectLat.begin(), ObjectLat.end())-0.01, 0.01+*std::max_element(ObjectLat.begin(), ObjectLat.end())); // Диапазон по широте
    objectLatPlot->replot();
    objectLongPlot->graph(0)->setData(ObjectLongTime, ObjectLong);
    objectLongPlot->xAxis->setRange(0, currentTime); // Диапазон по времени
    objectLongPlot->yAxis->setRange(*std::min_element(ObjectLong.begin(), ObjectLong.end())-0.01, 0.01+*std::max_element(ObjectLong.begin(), ObjectLong.end())); // Диапазон по долготе
    objectLongPlot->replot();
}

void MainWindow::update_plots_of_sat_coords(QCustomPlot *plot,QList<double> xData,QList<double> yData){
    double currentTime = QTime(0,0,0,0).secsTo(timer_time);
    //qDebug()<<"before"<<yData;
    if(currentTime!=xData.last()){
        xData.append(currentTime);
        yData.append(yData.last());
    }
    //qDebug()<<"after"<<yData;
    // Обновляем графики
    plot->graph(0)->setData(xData, yData);
    if(xData.size()<yData.size()){
        qDebug()<<yData<<xData;
    }
    plot->xAxis->setRange(0, currentTime); // Диапазон по времени
    plot->yAxis->setRange(*std::min_element(yData.begin(), yData.end())-pow(10,5), pow(10,5)+*std::max_element(yData.begin(), yData.end())); // Диапазон по широте
    plot->replot();
}
void MainWindow::show_selected_satellite(int id){
    chosenSatPlot->clearPlottables(); // Очищаем старые данные
    chosenSatPlot->clearItems(); // Очищаем старые элементы (текст и эллипсы)
    const Sputniks& sats = re->satellites;

    QCPGraph* objectPoint = chosenSatPlot->addGraph();
    objectPoint->setLineStyle(QCPGraph::lsNone);
    objectPoint->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::blue, 10));
    objectPoint->addData(sats.tab[id].SatX, sats.tab[id].SatY);

    QColor textcolor = chooseSatColor(sats.tab[id]);

    // Добавляем метку с номером спутника
    QCPItemText* textLabel = new QCPItemText(chosenSatPlot);
    textLabel->setPositionAlignment(Qt::AlignCenter);  // Выравнивание текста по центру
    textLabel->position->setCoords(sats.tab[id].SatX, sats.tab[id].SatY);  // Позиция метки (координаты спутника)
    if(id < 10){
        textLabel->setText("0" + QString::number(id));
    } else {
        textLabel->setText(QString::number(id));  // Текст метки (номер спутника)
    }
    textLabel->setFont(QFont("Arial", 14));  // Шрифт для текста
    textLabel->setColor(Qt::black);  // Цвет текста
    textLabel->setBrush(QBrush(textcolor));  // Фон
    textLabel->setPadding(QMargins(3, 3, 3, 3));  // Отступы вокруг текста
    // Обновляем диапазоны осей
    double margin = 0.1; // 10% от максимального расстояния

    chosenSatPlot->xAxis->setRange(-xRange * (1 + margin), xRange * (1 + margin));
    chosenSatPlot->yAxis->setRange(-yRange * (1 + margin), yRange * (1 + margin));

    // Включаем сетку и под-сетки
    chosenSatPlot->xAxis->grid()->setSubGridVisible(true);
    chosenSatPlot->yAxis->grid()->setSubGridVisible(true);

    chosenSatPlot->replot();  // Обновляем график
    QList <double> SatsTime = sats.tab[id].timing_history;
    update_plots_of_sat_coords(satXPlot,SatsTime,sats.tab[id].SatX_history);
    update_plots_of_sat_coords(satYPlot,SatsTime,sats.tab[id].SatY_history);
    update_plots_of_sat_coords(satZPlot,SatsTime,sats.tab[id].SatZ_history);
    satellite_is_chosen = id;
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    QString selectedText = ui->listView->model()->data(index, Qt::DisplayRole).toString();
    ui->statusbar->showMessage("Выбран спутник "+ selectedText);
    satellite_is_chosen = 0;
    show_selected_satellite(selectedText.toInt());
}

void MainWindow::updateClock() {
    //qDebug()<<clock_time.toString("hh:mm:ss");
    QString currentTime = timer_time.toString("hh:mm:ss");
    clock_time = clock_time.addSecs(1);
    timer_time=timer_time.addSecs(1);
    ui->clock->setText(currentTime);
    updateTable();
    plotSatelliteData();
    plotObject_Data();
    if(satellite_is_chosen!=0){
        show_selected_satellite(satellite_is_chosen);
    }
}

void MainWindow::on_listView_2_clicked(const QModelIndex &index)
{
    QString selectedText = ui->listView_2->model()->data(index, Qt::DisplayRole).toString();
    ui->statusbar->showMessage("Выбран спутник "+ selectedText);
    show_selected_satellite(selectedText.toInt());
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
    QTime curr_time = clock_time;
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
    QStringList keys;
    if(parced_data.getUpdateTime("SoGN")>parced_data.getUpdateTime("SoGK")){
        keys = {
            "Lat", "Long",
            "Alt", "SoGN", "TMGT"
        };
        list_results
            << QString::number(parced_data.Lat)
            << QString::number(parced_data.Long)
            << QString::number(parced_data.Alt)
            << QString::number(parced_data.SoGN*1.852)
            << QString::number(parced_data.TMGT);
    }else{
        keys = {
            "Lat", "Long",
            "Alt", "SoGK", "TMGT"
        };
        list_results
            << QString::number(parced_data.Lat)
            << QString::number(parced_data.Long)
            << QString::number(parced_data.Alt)
            << QString::number(parced_data.SoGK)
            << QString::number(parced_data.TMGT);
        //qDebug()<<"list_results"<<list_results;
    }
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
    //qDebug() << "Таблица заполнена";

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

