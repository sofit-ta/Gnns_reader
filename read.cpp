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
bool change_time_new_file = true;
QDateTime clock_time;
void read::updateParcingFile(QString fileName){
    path_to_parcing_file = fileName;
    last_read_line = -1;
    change_time_new_file = true;
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
    //qDebug()<<path_to_parcing_file;
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
                if(change_time_new_file){
                    qDebug()<<parts.at(0);
                    int hours = parts.at(0).left(2).toInt();
                    int minutes = parts.at(0).mid(2, 2).toInt();
                    int seconds = parts.at(0).mid(4, 2).toInt();
                    int milliseconds = parts.at(0).mid(7, 3).toInt();
                    if (hours >= 24) {
                        hours -= 24;  // Переход на следующий день
                    }
                    clock_time = QTime(hours, minutes, seconds, milliseconds);
                    qDebug()<<clock_time.toString();
                    change_time_new_file = false;
                } else{
                    int hours = parts.at(0).left(2).toInt();
                    int minutes = parts.at(0).mid(2, 2).toInt();
                    int seconds = parts.at(0).mid(4, 2).toInt();
                    int milliseconds = parts.at(0).mid(7, 3).toInt();
                    if (hours >= 24) {
                        hours -= 24;  // Переход на следующий день
                    }
                    if(QTime(hours, minutes, seconds, milliseconds)<=clock_time){
                        if (parts[1].contains(",") && parts[1].startsWith("$")) {
                            if (parts[1].startsWith("$PSTM")) {
                                // Передаем строку в метод для обработки данных
                                no_new_data = false;
                                PSTM_reading(parts[1].split(","));
                            } else if (parts[1].startsWith("$G")){
                                no_new_data = false;
                                NMEA_reading(parts[1].split(","));
                            }
                        }
                    } else{
                        last_read_line--;
                        counter--;
                    }
                }

            }
        }
    }
    file.close();
}
void read::Lat_Log(std::string key, double Coord){
    int degrees = static_cast<int>(Coord / 100);  // Get the degrees
    double minutes = Coord - (degrees * 100);     // Get the minutes

    data_info.updateValue(key,degrees + (minutes / 60.0));
}
void read::NMEA_reading(QStringList list_of_param) {
    QString syntax = list_of_param[0];
    syntax.removeAt(0);
    syntax.removeAt(0);
    syntax.removeAt(0);
    for(int i = 0; i < list_of_param.length(); ++i){
        if(list_of_param[i]==""){
            list_of_param[i]="inf";
        }
    }
    qDebug()<<syntax;
    if(syntax == "GGA"){
        qDebug()<<"yes yes";
        QString time_string = list_of_param.at(1);
        QString fmt_time_string = time_string.mid(0, 2) + ":" + time_string.mid(2, 2) + ":" + time_string.mid(4, 2) + "." + time_string.mid(7, 3);
        QTime timestampnotDouble = QTime::fromString(fmt_time_string, "hh:mm:ss.zzz");

        data_info.updateValue("Timestamp",static_cast<double>(timestampnotDouble.msecsSinceStartOfDay()));
        qDebug()<<data_info.Timestamp;
        Lat_Log("Lat",list_of_param[2].toDouble());
        data_info.updateValue("NS", list_of_param.at(3));
        Lat_Log("Long",list_of_param[4].toDouble());
        // data_info.updateValue("Lat",list_of_param[2].mid(0, 2).toDouble() + list_of_param.at(2).mid(2).toDouble() / 60);
        // data_info.updateValue("Long", list_of_param.at(3).mid(0, 3).toDouble() + list_of_param.at(3).mid(3).toDouble() / 60);

        data_info.updateValue("EW", list_of_param.at(5));
        data_info.updateValue("GPSQual",list_of_param.at(6).toDouble());
        data_info.updateValue("Sats",list_of_param.at(7).toDouble());
        data_info.updateValue("HDOP",list_of_param.at(8).toDouble());
        data_info.updateValue("Alt",list_of_param.at(9).toDouble());
        data_info.updateValue("AltVal", list_of_param.at(10));
        data_info.updateValue("GeoSep",list_of_param.at(11).toDouble());
        data_info.updateValue("GeoVal",list_of_param.at(12));
        data_info.updateValue("DGPSAge",list_of_param.at(13));

        int starPos = list_of_param[14].indexOf("*");
        int crlfPos = list_of_param[14].indexOf("\r\n");
        if (starPos != -1 && crlfPos != -1) {
            data_info.updateValue("Checksum",list_of_param[14].mid(starPos + 1, crlfPos - starPos - 1));
            data_info.updateValue("DGPSRef", list_of_param[14].left(starPos));
        }
        qDebug() << "Данные добавлены в data_info";
        if (mainWindow) {
            qDebug() << "entered";
            mainWindow->fill_the_table(false);  // Теперь мы можем вызвать fill_the_table()
        }
    }
    else if(syntax == "RMC"){
        QString time_string = list_of_param.at(1);
        QString fmt_time_string = time_string.mid(0, 2) + ":" + time_string.mid(2, 2) + ":" + time_string.mid(4, 2) + "." + time_string.mid(7, 3);
        QTime timestampnotDouble = QTime::fromString(fmt_time_string, "hh:mm:ss.zzz");
        data_info.updateValue("Timestamp",static_cast<double>(timestampnotDouble.msecsSinceStartOfDay()));
        data_info.updateValue("Status",list_of_param.at(2));
        data_info.updateValue("Lat",list_of_param[3].mid(0, 2).toDouble() + list_of_param.at(3).mid(2).toDouble() / 60);
        data_info.updateValue("NS", list_of_param.at(4));
        data_info.updateValue("Long", list_of_param.at(5).mid(0, 3).toDouble() + list_of_param.at(5).mid(3).toDouble() / 60);
        data_info.updateValue("EW", list_of_param.at(6));
        data_info.updateValue("SoGN",list_of_param.at(7).toDouble());
        data_info.updateValue("TMGT",list_of_param.at(8).toDouble());
        data_info.updateValue("Date",QDateTime::fromString(list_of_param[9], "ddMMyy").addYears(100));
        data_info.updateValue("MagVar",list_of_param.at(10).toDouble());
        data_info.updateValue("MagVarDir",list_of_param.at(11));


        int starPos = list_of_param[12].indexOf("*");
        int crlfPos = list_of_param[12].indexOf("\r\n");
        if (starPos != -1 && crlfPos != -1) {
            data_info.updateValue("Checksum",list_of_param[12].mid(starPos + 1, crlfPos - starPos - 1));
            data_info.updateValue("Mode", list_of_param[12].left(starPos));
        }
        qDebug() << "Данные добавлены в data_info";
        if (mainWindow) {
            qDebug() << "entered";
            mainWindow->fill_the_table(false);  // Теперь мы можем вызвать fill_the_table()
        }
    }
    else if(syntax == "GLL"){
        data_info.updateValue("Lat",list_of_param[1].mid(0, 2).toDouble() + list_of_param.at(1).mid(2).toDouble() / 60);
        data_info.updateValue("NS", list_of_param.at(2));
        data_info.updateValue("Long", list_of_param.at(3).mid(0, 3).toDouble() + list_of_param.at(3).mid(3).toDouble() / 60);
        data_info.updateValue("EW", list_of_param.at(4));

        QString time_string = list_of_param.at(5);
        QString fmt_time_string = time_string.mid(0, 2) + ":" + time_string.mid(2, 2) + ":" + time_string.mid(4, 2) + "." + time_string.mid(7, 3);
        QTime timestampnotDouble = QTime::fromString(fmt_time_string, "hh:mm:ss.zzz");
        data_info.updateValue("Timestamp",static_cast<double>(timestampnotDouble.msecsSinceStartOfDay()));
        data_info.updateValue("Status",list_of_param.at(6));

        int starPos = list_of_param[7].indexOf("*");
        int crlfPos = list_of_param[7].indexOf("\r\n");
        if (starPos != -1 && crlfPos != -1) {
            data_info.updateValue("Checksum",list_of_param[7].mid(starPos + 1, crlfPos - starPos - 1));
            data_info.updateValue("Mode", list_of_param[7].left(starPos));
        }
        qDebug() << "Данные добавлены в data_info";
        if (mainWindow) {
            qDebug() << "entered";
            mainWindow->fill_the_table(false);  // Теперь мы можем вызвать fill_the_table()
        }
    }
    else if(syntax == "VTG"){
        data_info.updateValue("TMGT",list_of_param.at(1).toDouble());
        data_info.updateValue("T",list_of_param.at(2));
        data_info.updateValue("TMGM",list_of_param.at(3).toDouble());
        data_info.updateValue("M",list_of_param.at(4));
        data_info.updateValue("SoGN",list_of_param.at(5).toDouble());
        data_info.updateValue("N",list_of_param.at(6));
        data_info.updateValue("SoGK",list_of_param.at(7).toDouble());
        data_info.updateValue("K",list_of_param.at(8));
        int starPos = list_of_param[8].indexOf("*");
        int crlfPos = list_of_param[8].indexOf("\r\n");
        if (starPos != -1 && crlfPos != -1) {
            data_info.updateValue("Checksum",list_of_param[8].mid(starPos + 1, crlfPos - starPos - 1));
            data_info.updateValue("D", list_of_param[8].left(starPos));
        }
        qDebug() << "Данные добавлены в data_info";
        if (mainWindow) {
            qDebug() << "entered";
            mainWindow->fill_the_table(false);  // Теперь мы можем вызвать fill_the_table()
        }
    }
    else{
        no_new_data = true;
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
    qDebug()<<syntax;
    for(int i = 0; i < list_of_param.length(); ++i){
        if(list_of_param[i]==""){
            list_of_param[i]="inf";
        }
    }
    if(syntax == "DIFF"){
        data_info.updateValue("ListSize",list_of_param[1]);
        data_info.updateValue("NCS",list_of_param[2]);
        int number_of_sats = data_info.ListSize;
        QVector<QVector<double>> sat_corr_list_array;
        int counter_of_arr_pos = 3;
        for (int counter_of_pairs = 0; counter_of_pairs < number_of_sats; ++counter_of_pairs) {
            QVector<double> pair;
            pair.append(list_of_param[counter_of_arr_pos].toDouble());
            pair.append(list_of_param[counter_of_arr_pos + 1].toDouble());
            sat_corr_list_array.append(pair);
            counter_of_arr_pos += 2;
        }
        data_info.updateValue("SatsCorrAvl",sat_corr_list_array);
        data_info.updateValue("Checksum",list_of_param[list_of_param.size() - 1]);
    }
    else if (syntax == "PV") {
        //qDebug() << list_of_param;

        QString time_string = list_of_param.at(1);
        QString fmt_time_string = time_string.mid(0, 2) + ":" + time_string.mid(2, 2) + ":" + time_string.mid(4, 2) + "." + time_string.mid(7, 3);
        QTime timestampnotDouble = QTime::fromString(fmt_time_string, "hh:mm:ss.zzz");

        data_info.updateValue("Timestamp",static_cast<double>(timestampnotDouble.msecsSinceStartOfDay()));
        qDebug()<<data_info.Timestamp;
        data_info.updateValue("Lat",list_of_param[2].mid(0, 2).toDouble() + list_of_param.at(2).mid(2).toDouble() / 60);
        data_info.updateValue("NS", list_of_param.at(3));

        data_info.updateValue("Long", list_of_param.at(4).mid(0, 3).toDouble() + list_of_param.at(4).mid(3).toDouble() / 60);

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
        if (mainWindow) {
            qDebug() << "entered";
            mainWindow->fill_the_table(false);  // Теперь мы можем вызвать fill_the_table()
        }
    }
    else{
        no_new_data = true;
    }
}

