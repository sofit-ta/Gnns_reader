// Включаем нужные заголовки
#include "read.h"
#include "mainwindow.h"
#include <QFile>
#include <QtCore/QDateTime>

// Конструктор класса read
read::read(QObject *parent, MainWindow *mainWindow)
    : QAbstractItemModel(parent),mainWindow(mainWindow) {}

ResultStructure data_info;
bool no_new_data=true;
int last_read_line = -1;
QString path_to_parcing_file;

void read::updateParcingFile(QString fileName){
    path_to_parcing_file = fileName;
}
// Метод для чтения файла
void read::file_reading(){
    if(path_to_parcing_file==""){
        mainWindow->statusBar()->showMessage(QString("!Не выбран файл для парсинга!"));
        mainWindow->on_open_file_triggered();
    }
    //QString filePath = "/Users/sofia/Desktop/science_work/parcing/resources/2021.05 logbook_demo.txt";

    // Открываем файл
    QFile file(path_to_parcing_file);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Не удалось открыть файл!";
        return;
    }

    QTextStream in(&file);

    no_new_data=true;
    int counter = 0;
    // Считываем строки файла, пока файл не закончится или кнопка не нажата
    while((!in.atEnd()) && (!MainWindow::button_is_pressed) &&(no_new_data)) {
        QString string = in.readLine();
        if (counter<=last_read_line){
            counter++;
            continue;
        }else{
            last_read_line++;
            counter++;

        }
        if (string.isEmpty()) {
            qDebug() << "Пустая строка";
            continue;
            // Пропускаем пустую строку
        }
        else{
            // Разделяем строку на части
            QStringList parts = string.split(";", Qt::SkipEmptyParts);

            // Проверяем, что строка содержит хотя бы 2 элемента
            if (parts.size() == 2) {
                data_info.gps_time = parts[0].toDouble();

                // Проверяем, содержит ли вторая часть символы и начинается ли с "$"
                if (parts[1].contains(",") && parts[1].startsWith("$")) {
                    if (parts[1].startsWith("$PSTM")) {
                        // Передаем строку в метод для обработки данных
                        PSTM_reading(parts[1].split(","));
                    }
                }
            }
        }
    }
}

// Метод для обработки команды PSTM
void read::PSTM_reading(QStringList list_of_param) {
    //if (list_of_param.size() < 23) {
    //    qDebug() << "Ошибка: недостаточно параметров!";
    //    return;
    //}

    QString syntax = list_of_param[0];
    syntax.remove("$PSTM");

    if (syntax == "PV") {
        //qDebug() << list_of_param;

        QString time_string = list_of_param.at(1);
        QString fmt_time_string = time_string.mid(0, 2) + ":" + time_string.mid(2, 2) + ":" + time_string.mid(4, 2) + "." + time_string.mid(7, 3);
        QTime timestampnotDouble = QTime::fromString(fmt_time_string, "hh:mm:ss.zzz");
        data_info.Timestamp = static_cast<double>(timestampnotDouble.msecsSinceStartOfDay());

        data_info.Lat = list_of_param[2].mid(0, 2).toDouble() + list_of_param.at(1).mid(2).toDouble() / 60;
        data_info.NS = list_of_param.at(3);
        data_info.Long = list_of_param.at(4).mid(0, 3).toDouble() + list_of_param.at(3).mid(3).toDouble() / 60;
        data_info.EW = list_of_param.at(5);
        data_info.Alt = list_of_param.at(6).toDouble();
        data_info.AltVal = list_of_param.at(7);
        data_info.VelN = list_of_param.at(8).toDouble();
        data_info.VelE = list_of_param.at(9).toDouble();
        data_info.VelV = list_of_param.at(10).toDouble();
        data_info.PCovN = list_of_param.at(11).toDouble();
        data_info.PCovNE = list_of_param.at(12).toDouble();
        data_info.PCovNV = list_of_param.at(13).toDouble();
        data_info.PCovE = list_of_param.at(14).toDouble();
        data_info.PCovEV = list_of_param.at(15).toDouble();
        data_info.PCovV = list_of_param.at(16).toDouble();
        data_info.VCovN = list_of_param.at(17).toDouble();
        data_info.VCovNE = list_of_param.at(18).toDouble();
        data_info.VCovNV = list_of_param.at(19).toDouble();
        data_info.VCovE = list_of_param.at(20).toDouble();
        data_info.VCovEV = list_of_param.at(21).toDouble();
        QStringList parts = list_of_param.at(22).split('*');
        if (parts.size() == 2) {
            data_info.VCovV = parts.at(0).toDouble();
            data_info.Checksum = parts.at(1);
        } else {
            qDebug() << "Ошибка: строка не содержит две части, разделенные символом '*'. Значение:" << list_of_param.at(22);
        }
        no_new_data = false;
        qDebug() << "Данные добавлены в data_info";
        if (mainWindow) {
            qDebug() << "entered";
            mainWindow->fill_the_table();  // Теперь мы можем вызвать fill_the_table()
        }
    }
}

