// Включаем нужные заголовки
#include "read.h"
#include "mainwindow.h"
#include <QFile>
#include <QtCore/QDateTime>

// Конструктор класса read
read::read(QObject *parent, MainWindow *mainWindow)
    : QAbstractItemModel(parent),mainWindow(mainWindow) {}

ResultStructure data_info;
Sputniks satellites;
bool no_new_data=true;
int last_read_line = -1;
QString path_to_parcing_file;
bool change_time_new_file = true;
QTime clock_time;
QTime curr_update_time;
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
    // if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    // {
    //     qDebug() << "Не удалось открыть файл!";
    //     return;
    // }

    // QTextStream in(&file);
    QStringList lines;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();
    }else{
        qDebug() << "Не удалось открыть файл!";
        return;
    }
    no_new_data=true;
    int counter = 0;
    if(lines.length()==last_read_line+1 && (curr_update_time.msecsTo(clock_time) >= 5000)){
        if(curr_update_time.msecsTo(clock_time) >= 10000){
        last_read_line++;
        }
        if (mainWindow) {
            qDebug() << "entered";
            mainWindow->fill_the_table(false);  // Теперь мы можем вызвать fill_the_table()
        }
    }
    // Считываем строки файла, пока файл не закончится или кнопка не нажата
    while((lines.length()>counter) && (!MainWindow::button_is_pressed)) {
        QString string = lines[counter];
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
                int hours = parts.at(0).left(2).toInt();
                int minutes = parts.at(0).mid(2, 2).toInt();
                int seconds = parts.at(0).mid(4, 2).toInt();
                int milliseconds = parts.at(0).mid(7, 3).toInt();
                if (hours >= 24) {
                    hours -= 24;
                }
                if(change_time_new_file){
                    qDebug()<<parts.at(0);
                    clock_time = QTime(hours, minutes, seconds, milliseconds);
                    qDebug()<<clock_time.toString();
                    change_time_new_file = false;
                    last_read_line--;
                } else{
                    if(QTime(hours, minutes, seconds, milliseconds)<=clock_time){
                        curr_update_time = QTime(hours, minutes, seconds, milliseconds);
                        qDebug() << "зашел в сообщение с временем"<<curr_update_time.toString()<<"в "<<clock_time.toString();
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
                    }
                    else{
                        qDebug() << "noo"<<curr_update_time.toString()<<"в "<<clock_time.toString();
                        last_read_line--;
                    }
                }
                if (mainWindow) {
                    qDebug() << "entered";
                    mainWindow->fill_the_table(false);  // Теперь мы можем вызвать fill_the_table()
                }
            }
        }
    }
}

void read::Lat_Log(std::string key, double Coord){
    int degrees = static_cast<int>(Coord / 100);
    double minutes = Coord - (degrees * 100);

    data_info.updateValue(key,degrees + (minutes / 60.0));
}
QString read::Checksum_reading_STR(QString str,std::string key){
    int starPos = str.indexOf("*");
    int crlfPos = str.indexOf("\r\n");
    if (starPos != -1 && crlfPos != -1) {
        data_info.updateValue("Checksum",str.mid(starPos + 1, crlfPos - starPos - 1));
        if(key!=""){
            data_info.updateValue(key, str.left(starPos));
        }
        return str.left(starPos);
    }
    return "";
}
void read::Checksum_reading_Double(QString str, std::string key){
    int starPos = str.indexOf("*");
    int crlfPos = str.indexOf("\r\n");
    if (starPos != -1 && crlfPos != -1) {
        data_info.updateValue("Checksum",str.mid(starPos + 1, crlfPos - starPos - 1));
        if(key!=""){
            data_info.updateValue(key, str.left(starPos).toDouble());}
    }
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
        Checksum_reading_STR(list_of_param.at(14),"DGPSRef");
        QTime timestampnotDouble = QTime::fromString(list_of_param.at(1), "hhmmss.zzz");
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
    }
    else if(syntax == "RMC"){
        Checksum_reading_STR(list_of_param.at(12),"Mode");
        QTime timestampnotDouble = QTime::fromString(list_of_param.at(1), "hhmmss.zzz");
        data_info.updateValue("Timestamp",static_cast<double>(timestampnotDouble.msecsSinceStartOfDay()));
        data_info.updateValue("Status",list_of_param.at(2));
        Lat_Log("Lat",list_of_param[3].toDouble());
        data_info.updateValue("NS", list_of_param.at(4));
        Lat_Log("Long",list_of_param[5].toDouble());
        data_info.updateValue("EW", list_of_param.at(6));
        data_info.updateValue("SoGN",list_of_param.at(7).toDouble());
        data_info.updateValue("TMGT",list_of_param.at(8).toDouble());
        data_info.updateValue("Date",QDateTime::fromString(list_of_param[9], "ddMMyy").addYears(100));
        data_info.updateValue("MagVar",list_of_param.at(10).toDouble());
        data_info.updateValue("MagVarDir",list_of_param.at(11));

    }
    else if(syntax == "GLL"){
        Checksum_reading_STR(list_of_param.at(7),"Mode");
        Lat_Log("Lat",list_of_param[1].toDouble());
        data_info.updateValue("NS", list_of_param.at(2));
        Lat_Log("Long",list_of_param[3].toDouble());
        data_info.updateValue("EW", list_of_param.at(4));

        QTime timestampnotDouble = QTime::fromString(list_of_param.at(5), "hhmmss.zzz");
        data_info.updateValue("Timestamp",static_cast<double>(timestampnotDouble.msecsSinceStartOfDay()));
        data_info.updateValue("Status",list_of_param.at(6));
    }
    else if(syntax == "VTG"){
        Checksum_reading_STR(list_of_param.at(8),"D");
        data_info.updateValue("TMGT",list_of_param.at(1).toDouble());
        data_info.updateValue("T",list_of_param.at(2));
        data_info.updateValue("TMGM",list_of_param.at(3).toDouble());
        data_info.updateValue("M",list_of_param.at(4));
        data_info.updateValue("SoGN",list_of_param.at(5).toDouble());
        data_info.updateValue("N",list_of_param.at(6));
        data_info.updateValue("SoGK",list_of_param.at(7).toDouble());
        data_info.updateValue("K",list_of_param.at(8));
    }
    else if(syntax == "GST"){
        Checksum_reading_Double(list_of_param[8],"AltErr");
        QTime timestampnotDouble = QTime::fromString(list_of_param.at(1), "hhmmss.zzz");
        data_info.updateValue("Timestamp",static_cast<double>(timestampnotDouble.msecsSinceStartOfDay()));
        data_info.updateValue("EHPE",list_of_param.at(2).toDouble());
        data_info.updateValue("SemiMajor",list_of_param.at(3).toDouble());
        data_info.updateValue("SemiMinor",list_of_param.at(4).toDouble());
        data_info.updateValue("Angle",list_of_param.at(5).toDouble());
        data_info.updateValue("LatErr",list_of_param.at(6).toDouble());
        data_info.updateValue("LonErr",list_of_param.at(7).toDouble());
    }
    else if(syntax == "GBS"){
        Checksum_reading_STR(list_of_param[8],"");
        QTime timestampnotDouble = QTime::fromString(list_of_param.at(1), "hhmmss.zzz");
        data_info.updateValue("Timestamp",static_cast<double>(timestampnotDouble.msecsSinceStartOfDay()));
        data_info.updateValue("LatErr",list_of_param.at(2).toDouble());
        data_info.updateValue("LonErr",list_of_param.at(3).toDouble());
        data_info.updateValue("AltErr",list_of_param.at(4).toDouble());
        data_info.updateValue("SatPRN",list_of_param.at(5).toDouble());
        data_info.updateValue("Res",list_of_param.at(7));
        // Field #5 'st.Prob' is empty (not supported) in accordance with
        // en.DM00398983 (for NMEA 0183 Rev 3.1 (Default)).
    }
    else if(syntax == "GNS"){
        Checksum_reading_STR(list_of_param[13],"");
        QTime timestampnotDouble = QTime::fromString(list_of_param.at(1), "hhmmss.zzz");
        data_info.updateValue("Timestamp",static_cast<double>(timestampnotDouble.msecsSinceStartOfDay()));
        Lat_Log("Lat",list_of_param[2].toDouble());
        data_info.updateValue("NS", list_of_param.at(3));
        Lat_Log("Long",list_of_param[4].toDouble());
        data_info.updateValue("EW", list_of_param.at(5));
        data_info.updateValue("ModeIndicator", list_of_param.at(6));
        data_info.updateValue("Sats",list_of_param.at(7).toDouble());
        data_info.updateValue("HDOP",list_of_param.at(8).toDouble());
        data_info.updateValue("Alt",list_of_param.at(9).toDouble());
        data_info.updateValue("GeoSep",list_of_param[9].toDouble());
        // Fields #10 & #11 ('st.DGNSSAge' & 'st.DGNSSRef') are empty (not supported) in accordance with
        // en.DM00398983.
    }
    else if(syntax == "DTM"){
        Checksum_reading_STR(list_of_param[8],"ReferenceDatumCode");
        data_info.updateValue("LocalDatumCode", list_of_param.at(1));
        data_info.updateValue("LocalDatumCodeID",list_of_param.at(2).toDouble());
        data_info.updateValue("LatOffset",list_of_param.at(2).toDouble());
        data_info.updateValue("NS", list_of_param.at(1));
        data_info.updateValue("LongOffset",list_of_param.at(2).toDouble());
        data_info.updateValue("EW", list_of_param.at(5));
        data_info.updateValue("AltOffset",list_of_param.at(2).toDouble());
    }
    else if(syntax == "GSA"){
        QString vdop = Checksum_reading_STR(list_of_param.at(17),"");
        data_info.updateValue("ModeIndicator", list_of_param.at(1));
        data_info.updateValue("CurrentMode", list_of_param.at(2).toDouble());
        QList<int> id_of_sputniks;
        for(int position = 3; position < 15; ++position){
            id_of_sputniks.append(list_of_param.at(position).toInt());
        }
        for (double id : id_of_sputniks) {
            satellites.updateSputnik(id,"PDOP",list_of_param.at(15).toDouble());
            satellites.updateSputnik(id,"HDOP",list_of_param.at(16).toDouble());
            satellites.updateSputnik(id,"VDOP",vdop.toDouble());
        }
    }
    else if(syntax == "GSV"){
        QString SatxCN0 = Checksum_reading_STR(list_of_param.at(19),"");
        data_info.updateValue("GSVAmount",list_of_param.at(1).toDouble());
        data_info.updateValue("GSVNumber",list_of_param.at(2).toDouble());
        data_info.updateValue("TotSats",list_of_param.at(3).toDouble());
        for (int position = 4; position < 19; position=position+4) {
            int id = list_of_param.at(position).toInt();
            satellites.updateSputnik(id,"Elev",list_of_param.at(position+1).toInt());
            satellites.updateSputnik(id,"Azimut",list_of_param.at(position+2).toInt());
            if(position+3==19){
                satellites.updateSputnik(id,"CN0",SatxCN0.toInt());
            }else{
                satellites.updateSputnik(id,"CN0",list_of_param.at(position+3).toInt());
            }

        }
    }
    else{
        no_new_data = true;
        return;
    }
    qDebug() << "new Данные добавлены в data_info";
    // if (mainWindow) {
    //     qDebug() << "entered";
    //     mainWindow->fill_the_table(false);  // Теперь мы можем вызвать fill_the_table()
    // }
}
// Метод для обработки команды PSTM
void read::PSTM_reading(QStringList list_of_param) {


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
        data_info.updateValue("Checksum",list_of_param[list_of_param.size() - 1]); // Checksum считается необычно
    }
    else if (syntax == "PV") {
        //qDebug() << list_of_param;

        Checksum_reading_Double(list_of_param.at(22),"VCovV");
        QTime timestampnotDouble = QTime::fromString(list_of_param.at(1), "hhmmss.zzz");
        data_info.updateValue("Timestamp",static_cast<double>(timestampnotDouble.msecsSinceStartOfDay()));

        data_info.updateValue("Timestamp",static_cast<double>(timestampnotDouble.msecsSinceStartOfDay()));
        qDebug()<<data_info.Timestamp;
        Lat_Log("Lat",list_of_param[2].toDouble());
        data_info.updateValue("NS", list_of_param.at(3));
        Lat_Log("Long",list_of_param[4].toDouble());
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
    }
    else if(syntax=="PVRAW"){
        Checksum_reading_Double(list_of_param.at(16),"VCovV");
        QTime timestampnotDouble = QTime::fromString(list_of_param.at(1), "hhmmss.zzz");
        data_info.updateValue("Timestamp",static_cast<double>(timestampnotDouble.msecsSinceStartOfDay()));
        Lat_Log("Lat",list_of_param[2].toDouble());
        data_info.updateValue("NS", list_of_param.at(3));
        Lat_Log("Long",list_of_param[4].toDouble());
        data_info.updateValue("EW", list_of_param.at(5));
        data_info.updateValue("GPSQual",list_of_param.at(6).toDouble());
        data_info.updateValue("Sats",list_of_param.at(7).toDouble());
        data_info.updateValue("HDOP",list_of_param.at(8).toDouble());
        data_info.updateValue("Alt",list_of_param.at(9).toDouble());
        data_info.updateValue("AltVal", list_of_param.at(10));
        data_info.updateValue("GeoSep",list_of_param[11].toDouble());
        data_info.updateValue("GeoVal", list_of_param.at(12));
        data_info.updateValue("VelN", list_of_param.at(13).toDouble());
        data_info.updateValue("VelE", list_of_param.at(14).toDouble());
        data_info.updateValue("VelV", list_of_param.at(15).toDouble());
    }
    else if(syntax == "TS"){
        QString PredTD = Checksum_reading_STR(list_of_param.at(23),"");
        int id = list_of_param.at(2).toInt();
        satellites.updateSputnik(id,"DSPDat",list_of_param.at(1).toInt());
        satellites.updateSputnik(id,"PsR",list_of_param.at(3).toDouble());
        satellites.updateSputnik(id,"Freq",list_of_param.at(4).toDouble());
        satellites.updateSputnik(id,"PLF",list_of_param.at(5).toInt());
        satellites.updateSputnik(id,"CN0",list_of_param.at(6).toInt());
        satellites.updateSputnik(id,"TTim",list_of_param.at(7).toDouble());
        satellites.updateSputnik(id,"SatDat",list_of_param.at(8).toInt());
        satellites.updateSputnik(id,"SatX",list_of_param.at(9).toDouble());
        satellites.updateSputnik(id,"SatY",list_of_param.at(10).toDouble());
        satellites.updateSputnik(id,"SatZ",list_of_param.at(11).toDouble());
        satellites.updateSputnik(id,"VelX",list_of_param.at(12).toDouble());
        satellites.updateSputnik(id,"VelY",list_of_param.at(13).toDouble());
        satellites.updateSputnik(id,"VelZ",list_of_param.at(14).toDouble());
        satellites.updateSputnik(id,"SRC",list_of_param.at(15).toDouble());
        satellites.updateSputnik(id,"AC",list_of_param.at(16).toDouble());
        satellites.updateSputnik(id,"DifDat",list_of_param.at(17).toInt());
        satellites.updateSputnik(id,"DRC",list_of_param.at(18).toDouble());
        satellites.updateSputnik(id,"DRRC",list_of_param.at(19).toDouble());
        satellites.updateSputnik(id,"PredAvl",list_of_param.at(20).toInt());
        satellites.updateSputnik(id,"PredAge",list_of_param.at(21).toInt());
        satellites.updateSputnik(id,"PredEph",list_of_param.at(22).toInt());
        satellites.updateSputnik(id,"PredTD",PredTD.toInt());
    }
    else if(syntax== "SAT"){
        QString Satz = Checksum_reading_STR(list_of_param.at(6),"");
        int id = list_of_param.at(1).toInt();
        satellites.updateSputnik(id,"PsR",list_of_param.at(2).toDouble());
        satellites.updateSputnik(id,"Freq",list_of_param.at(3).toDouble());
        satellites.updateSputnik(id,"SatX",list_of_param.at(4).toDouble());
        satellites.updateSputnik(id,"SatY",list_of_param.at(5).toDouble());
        satellites.updateSputnik(id,"SatZ",Satz.toDouble());
    }
    else{
        no_new_data = true;
        return;
    }
    qDebug() << "new Данные добавлены в data_info";
    // if (mainWindow) {
    //     qDebug() << "entered";
    //     mainWindow->fill_the_table(false);  // Теперь мы можем вызвать fill_the_table()
    // }
}

