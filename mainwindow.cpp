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

    //график для окна положения объекта
    objectSatPlot = new QCustomPlot(ui->objectLocation);
    objectSatPlot->setParent(ui->objectLocation); // Связываем с виджетом
    objectSatPlot->resize(ui->objectLocation->size()); // Устанавливаем размер
    set_the_plot(objectSatPlot);

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
    plot->xAxis->setLabel("X");
    plot->yAxis->setLabel("Y");
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
            //satellitePoints->addData(sats.tab[id].SatX,sats.tab[id].SatY);
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

    currSatPlot->replot(); // Обновляем график
}
void MainWindow::addRotatedRectangle(QCustomPlot *customPlot, double x, double y, double width, double height, double angleDegrees)
{
    // Углы прямоугольника относительно центра
    QVector<QPointF> corners = {
        QPointF(-width / 2, -height / 2), // Левый нижний угол
        QPointF(width / 2, -height / 2),  // Правый нижний угол
        QPointF(width / 2, height / 2),   // Правый верхний угол
        QPointF(-width / 2, height / 2)  // Левый верхний угол
    };

    // Преобразуем угол из градусов в радианы
    double angleRadians = angleDegrees * M_PI / 180.0;

    // Матрица поворота: [cos(a) -sin(a); sin(a) cos(a)]
    QTransform rotationMatrix;
    rotationMatrix.rotate(angleDegrees);

    // Применяем поворот и сдвигаем прямоугольник в нужное место
    for (auto &corner : corners)
    {
        corner = rotationMatrix.map(corner); // Применяем поворот
        corner += QPointF(x, y);            // Смещаем центр
    }

    // Соединяем углы линиями
    for (int i = 0; i < 4; ++i)
    {
        QCPItemLine *line = new QCPItemLine(customPlot);
        line->start->setCoords(corners[i].x(), corners[i].y());
        line->end->setCoords(corners[(i + 1) % 4].x(), corners[(i + 1) % 4].y());
        line->setPen(QPen(Qt::red, 2)); // Устанавливаем цвет и толщину линии
    }
}

void MainWindow::addRotatedTriangle(QCustomPlot *customPlot, double centerX, double centerY, double base, double height, double angleDegrees)
{
    // Преобразуем угол из градусов в радианы
    double angleRadians = angleDegrees * M_PI / 180.0;

    // Координаты вершин относительно центра (до поворота)
    QPointF top(0, height);                   // Верхняя вершина
    QPointF bottomLeft(-base / 2, 0);         // Левая вершина основания
    QPointF bottomRight(base / 2, 0);         // Правая вершина основания

    // Матрица поворота
    QTransform rotationMatrix;
    rotationMatrix.rotate(angleDegrees);

    // Применяем поворот и сдвигаем треугольник в нужное место
    top = rotationMatrix.map(top) + QPointF(centerX, centerY);
    bottomLeft = rotationMatrix.map(bottomLeft) + QPointF(centerX, centerY);
    bottomRight = rotationMatrix.map(bottomRight) + QPointF(centerX, centerY);

    // Добавляем стороны треугольника
    QCPItemLine *side1 = new QCPItemLine(customPlot);
    side1->start->setCoords(top.x(), top.y());
    side1->end->setCoords(bottomLeft.x(), bottomLeft.y());
    side1->setPen(QPen(Qt::green, 2)); // Цвет и толщина линии

    QCPItemLine *side2 = new QCPItemLine(customPlot);
    side2->start->setCoords(top.x(), top.y());
    side2->end->setCoords(bottomRight.x(), bottomRight.y());
    side2->setPen(QPen(Qt::green, 2));

    QCPItemLine *baseLine = new QCPItemLine(customPlot);
    baseLine->start->setCoords(bottomLeft.x(), bottomLeft.y());
    baseLine->end->setCoords(bottomRight.x(), bottomRight.y());
    baseLine->setPen(QPen(Qt::green, 2));
}

void MainWindow::reCalculateSatCords(int id,double deltaX,double deltaY){
    Sputniks& sats = re->satellites;
    QTime curr_time = re->clock_time;
    if(sats.tab[id].last_updated.msecsTo(curr_time) <= 1000){
        sats.tab[id].SatXCalculated=sats.tab[id].SatX;
        sats.tab[id].SatYCalculated=sats.tab[id].SatY;
    }else{
        sats.tab[id].SatXCalculated-=deltaX/3600;
        sats.tab[id].SatYCalculated-=deltaY/3600;

    }
}

void MainWindow::plotObject_SatelliteData() {
    const Sputniks& sats = re->satellites;
    objectSatPlot->clearPlottables(); // Очищаем старые данные
    objectSatPlot->clearItems(); // Очищаем старые элементы (текст и эллипсы)

    QPair<double, double> objectPos = {0.0, 0.0}; // Точка объекта

    // Добавляем точку объекта
    QCPGraph* objectPoint = objectSatPlot->addGraph();
    objectPoint->setLineStyle(QCPGraph::lsNone);
    objectPoint->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, Qt::blue, 10));
    objectPoint->addData(objectPos.first, objectPos.second);


    const ResultStructure& parced_data = re->data_info;
    double v;
    double course = parced_data.TMGT;
    if(parced_data.getUpdateTime("SoGN")>parced_data.getUpdateTime("SoGK")){
        v= parced_data.SoGN *1.854;
    }else{
        v= parced_data.SoGK;
    }
    //addRotatedRectangle(objectSatPlot, 0, 0, 5*pow(10,6), 3*pow(10,6), course);
    //addRotatedTriangle(objectSatPlot, 0, 0, 5*pow(10,6), 3*pow(10,6), course);
    course = qDegreesToRadians(course);
    double deltaX = v* cos(course);
    double deltaY = v* sin(course);
    double xMin = 0, xMax = 0, yMin = 0, yMax = 0;
    for (int id = 1; id < 93; id++) {
        if(sats.tab[id].Status){
            reCalculateSatCords(id,deltaX,deltaY);
            QColor textcolor = chooseSatColor(sats.tab[id]);
            // Добавляем метку с номером спутника
            QCPItemText* textLabel = new QCPItemText(objectSatPlot);
            textLabel->setPositionAlignment(Qt::AlignCenter);  // Выравнивание текста по центру
            textLabel->position->setCoords(sats.tab[id].SatXCalculated,sats.tab[id].SatYCalculated);  // Позиция метки (координаты спутника)
            if(id<10){
                textLabel->setText("0"+QString::number(id));
            }else{
                textLabel->setText(QString::number(id));  // Текст метки (номер спутника)
            }
            textLabel->setFont(QFont("Arial", 14));  // Шрифт для текста
            textLabel->setColor(Qt::black);  // Цвет текста
            textLabel->setBrush(QBrush(textcolor));  //фон
            textLabel->setPadding(QMargins(3, 3, 3, 3));  // Отступы вокруг текста (слева, сверху, справа, снизу)
            xMin = qMin(xMin, sats.tab[id].SatXCalculated);
            xMax = qMax(xMax, sats.tab[id].SatXCalculated);
            yMin = qMin(yMin, sats.tab[id].SatYCalculated);
            yMax = qMax(yMax, sats.tab[id].SatYCalculated);
        }
    }
    // Устанавливаем диапазоны осей
    double margin = 0.1; // 10% от максимального расстояния
    xRange = qMax(fabs(xMax), fabs(xMin));
    yRange = qMax(fabs(yMax), fabs(yMin));

    objectSatPlot->xAxis->setRange(-xRange * (1 + margin), xRange * (1 + margin));
    objectSatPlot->yAxis->setRange(-yRange * (1 + margin), yRange * (1 + margin));

    objectSatPlot->xAxis->grid()->setSubGridVisible(true); // Включаем под-сетки
    objectSatPlot->yAxis->grid()->setSubGridVisible(true); // Включаем под-сетки

    objectSatPlot->replot(); // Обновляем график
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
}
void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    QString selectedText = ui->listView->model()->data(index, Qt::DisplayRole).toString();
    ui->statusbar->showMessage("Выбран спутник "+ selectedText);
    show_selected_satellite(selectedText.toInt());
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
    qDebug() << "Таблица заполнена";
    if (!first_time){
        qDebug() << "graph заполнен";
        plotSatelliteData();
        plotObject_SatelliteData();
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

