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
    last_read_line = -1;
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
    qDebug()<<path_to_parcing_file;
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
    file.close();
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

        data_info.updateValue("Timestamp",static_cast<double>(timestampnotDouble.msecsSinceStartOfDay()));
        qDebug()<<data_info.Timestamp;
        data_info.updateValue("Lat",list_of_param[2].mid(0, 2).toDouble() + list_of_param.at(1).mid(2).toDouble() / 60);
        data_info.updateValue("NS", list_of_param.at(3));

        data_info.updateValue("Long", list_of_param.at(4).mid(0, 3).toDouble() + list_of_param.at(3).mid(3).toDouble() / 60);

        data_info.updateValue("EW", list_of_param.at(5));
        data_info.updateValue("Alt", list_of_param.at(6).toDouble());
        data_info.updateValue("AltVal", list_of_param.at(7));
        data_info.updateValue("VelN", list_of_param.at(8).toDouble());
        data_info.updateValue("VelE", list_of_param.at(9).toDouble());
        data_info.updateValue("VelV", list_of_param.at(10).toDouble());

        // Position covariance components
        data_info.updateValue("PCovN", list_of_param.at(11).toDouble());
        data_info.updateValue("PCovNE", list_of_param.at(12).toDouble());
        data_info.updateValue("PCovNV", list_of_param.at(13).toDouble());
        data_info.updateValue("PCovE", list_of_param.at(14).toDouble());
        data_info.updateValue("PCovEV", list_of_param.at(15).toDouble());
        data_info.updateValue("PCovV", list_of_param.at(16).toDouble());

        // Velocity covariance components
        data_info.updateValue("VCovN", list_of_param.at(17).toDouble());
        data_info.updateValue("VCovNE", list_of_param.at(18).toDouble());
        data_info.updateValue("VCovNV", list_of_param.at(19).toDouble());
        data_info.updateValue("VCovE", list_of_param.at(20).toDouble());
        data_info.updateValue("VCovEV", list_of_param.at(21).toDouble());
        // Handle last parameter with potential checksum
        QStringList parts = list_of_param.at(22).split('*');
        if (parts.size() == 2) {
            data_info.updateValue("VCovV", parts.at(0).toDouble());
            data_info.updateValue("Checksum", parts.at(1));
        } else {
            qDebug() << "Error: The string does not contain two parts separated by '*'. Value:" << list_of_param.at(22);
        }
        qDebug() << "Данные добавлены в data_info";
        no_new_data = false;
        if (mainWindow) {
            qDebug() << "entered";
            mainWindow->fill_the_table(false);  // Теперь мы можем вызвать fill_the_table()
        }
    }
}

