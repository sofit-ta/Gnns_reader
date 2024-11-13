#ifndef READ_H
#define READ_H



#include <QAbstractItemModel>
#include <QVector>
#include <QString>
#include <QDebug>
#include <QtCore/qdatetime.h>
#include <iostream>




class MainWindow;
struct ResultStructure {
    double gps_time = 0.0;
    double Timestamp = 0.0;
    double Lat = 0.0;
    QString NS = "";
    double Long = 0.0;
    QString EW = "";
    double Alt = 0.0;
    QString AltVal = "";
    double VelN = 0.0;
    double VelE = 0.0;
    double VelV = 0.0;
    double PCovN = 0.0;
    double PCovNE = 0.0;
    double PCovNV = 0.0;
    double PCovE = 0.0;
    double PCovEV = 0.0;
    double PCovV = 0.0;
    double VCovN = 0.0;
    double VCovNE = 0.0;
    double VCovNV = 0.0;
    double VCovE = 0.0;
    double VCovEV = 0.0;
    double VCovV = 0.0;
    QString Checksum = "";

    QString Status = "";
    double ListSize= 0.0;
    double NCS = 0.0;
    QVector<QVector<double>> SatsCorrAvl;

    double GPSQual = 0.0;
    double Sats = 0.0;
    double HDOP = 0.0;
    double GeoSep = 0.0;
    QString GeoVal = "";
    QString DGPSAge = "";
    QString DGPSRef = "";

    double Speed = 0.0;
    double TrackGood = 0.0;
    QDateTime Date;
    double MagVar = 0.0;
    QString MagVarDir = "";

    double TMGT = 0.0;
    QString T = "";
    double TMGM = 0.0;
    QString M = "";
    double SoGN = 0.0;
    QString N = "";
    double SoGK = 0.0;
    QString K = "";
    QString D = "";

    double EHPE;
    double SemiMajor;
    double SemiMinor;
    double Angle;
    double LatErr;
    double LonErr;
    double AltErr;

    double SatPRN;
    QString Res;

    QString ModeIndicator;

    QString LocalDatumCode;
    QString LocalDatumCodeID;
    double LatOffset;
    double LongOffset;
    double AltOffset;
    QString ReferenceDatumCode;

    std::unordered_map<std::string, QDateTime> lastUpdated;

    using ValueType = std::variant<QString, double,QVector<double>,QDateTime,QVector<QVector<double>>>;

    // Обновитель
    std::unordered_map<std::string, std::function<void(ValueType)>> updater;



    ResultStructure() {
        // Initialize the updater map with lambda functions for each member
        updater["Timestamp"] = [this](ValueType value) {Timestamp = std::get<double>(value); };
        updater["Lat"] = [this](ValueType value) {Lat = std::get<double>(value); };
        updater["Long"] = [this](ValueType value) {Long = std::get<double>(value); };
        updater["Alt"] = [this](ValueType value) {Alt = std::get<double>(value); };
        updater["VelN"] = [this](ValueType value) {VelN = std::get<double>(value); };
        updater["VelE"] = [this](ValueType value) {VelE = std::get<double>(value); };
        updater["VelV"] = [this](ValueType value) {VelV = std::get<double>(value); };
        updater["NS"] = [this](ValueType value) { NS = std::get<QString>(value); };
        updater["EW"] = [this](ValueType value) { EW = std::get<QString>(value); };
        updater["AltVal"] = [this](ValueType value) { AltVal = std::get<QString>(value); };
        updater["PCovN"] = [this](ValueType value) { PCovN = std::get<double>(value); };
        updater["PCovNE"] = [this](ValueType value) { PCovNE = std::get<double>(value); };

        updater["PCovN"] = [this](ValueType value) { PCovN = std::get<double>(value); };
        updater["PCovNE"] = [this](ValueType value) { PCovNE = std::get<double>(value); };
        updater["PCovNV"] = [this](ValueType value) { PCovNV = std::get<double>(value); };
        updater["PCovE"] = [this](ValueType value) { PCovE = std::get<double>(value); };
        updater["PCovEV"] = [this](ValueType value) { PCovEV = std::get<double>(value); };
        updater["PCovV"] = [this](ValueType value) { PCovV = std::get<double>(value); };
        updater["VCovN"] = [this](ValueType value) { VCovN = std::get<double>(value); };
        updater["VCovNE"] = [this](ValueType value) { VCovNE = std::get<double>(value); };
        updater["VCovNV"] = [this](ValueType value) { VCovNV = std::get<double>(value); };
        updater["VCovE"] = [this](ValueType value) { VCovE = std::get<double>(value); };
        updater["VCovEV"] = [this](ValueType value) { VCovEV = std::get<double>(value); };
        updater["VCovV"] = [this](ValueType value) { VCovV = std::get<double>(value); };
        updater["Checksum"] = [this](ValueType value) { Checksum = std::get<QString>(value); };

        updater["Status"] = [this](ValueType value) { Status = std::get<QString>(value); };
        updater["ListSize"] = [this](ValueType value) { ListSize = std::get<double>(value); };
        updater["NCS"] = [this](ValueType value) { NCS = std::get<double>(value); };
        updater["SatsCorrAvl"] = [this](ValueType value) { SatsCorrAvl = std::get<QVector<QVector<double>>>(value); };

        updater["GPSQual"] = [this](ValueType value) { GPSQual = std::get<double>(value); };
        updater["Sats"] = [this](ValueType value) { Sats = std::get<double>(value); };
        updater["HDOP"] = [this](ValueType value) { HDOP = std::get<double>(value); };
        updater["GeoSep"] = [this](ValueType value) { GeoSep = std::get<double>(value); };
        updater["GeoVal"] = [this](ValueType value) { GeoVal = std::get<QString>(value); };
        updater["DGPSAge"] = [this](ValueType value) { DGPSAge = std::get<QString>(value); };
        updater["DGPSRef"] = [this](ValueType value) { DGPSRef = std::get<QString>(value); };

        updater["TrackGood"] = [this](ValueType value) { TrackGood = std::get<double>(value); };
        updater["Date"] = [this](ValueType value) { Date = std::get<QDateTime>(value); };
        updater["MagVar"] = [this](ValueType value) { MagVar = std::get<double>(value); };
        updater["MagVarDir"] = [this](ValueType value) { MagVarDir = std::get<QString>(value); };

        updater["TMGT"] = [this](ValueType value) { TMGT = std::get<double>(value); };
        updater["TMGM"] = [this](ValueType value) { TMGM = std::get<double>(value); };
        updater["SoGN"] = [this](ValueType value) { SoGN = std::get<double>(value); };
        updater["SoGK"] = [this](ValueType value) { SoGK = std::get<double>(value); };
        updater["T"] = [this](ValueType value) { T = std::get<QString>(value); };
        updater["M"] = [this](ValueType value) { M = std::get<QString>(value); };
        updater["N"] = [this](ValueType value) { N = std::get<QString>(value); };
        updater["K"] = [this](ValueType value) { K = std::get<QString>(value); };
        updater["D"] = [this](ValueType value) { D = std::get<QString>(value); };

        updater["EHPE"] = [this](ValueType value) { EHPE = std::get<double>(value); };
        updater["SemiMajor"] = [this](ValueType value) { SemiMajor = std::get<double>(value); };
        updater["SemiMinor"] = [this](ValueType value) { SemiMinor = std::get<double>(value); };
        updater["Angle"] = [this](ValueType value) { Angle = std::get<double>(value); };
        updater["LatErr"] = [this](ValueType value) { LatErr = std::get<double>(value); };
        updater["LonErr"] = [this](ValueType value) { LonErr = std::get<double>(value); };
        updater["AltErr"] = [this](ValueType value) { AltErr = std::get<double>(value); };

        updater["SatPRN"] = [this](ValueType value) { SatPRN = std::get<double>(value); };
        updater["Res"] = [this](ValueType value) { Res = std::get<QString>(value); };

        updater["ModeIndicator"] = [this](ValueType value) { ModeIndicator = std::get<QString>(value); };

        updater["LocalDatumCode"] = [this](ValueType value) { LocalDatumCode = std::get<QString>(value); };
        updater["LocalDatumCodeID"] = [this](ValueType value) { LocalDatumCodeID = std::get<QString>(value); };
        updater["LatOffset"] = [this](ValueType value) { LatOffset = std::get<double>(value); };
        updater["LongOffset"] = [this](ValueType value) { LongOffset = std::get<double>(value); };
        updater["AltOffset"] = [this](ValueType value) { AltOffset = std::get<double>(value); };
        updater["ReferenceDatumCode"] = [this](ValueType value) { ReferenceDatumCode = std::get<QString>(value); };
    }

    void updateValue(const std::string& key, ValueType value) {
        auto it = updater.find(key);
        //std::visit([](auto&& arg) { qDebug() << "Type:" << typeid(arg).name() << "Value:" << arg; }, value);
        if (std::holds_alternative<QString>(value)) {
            QString strValue = std::get<QString>(value);
            //qDebug() << "QString value:" << strValue;
            if (strValue=="inf" &&lastUpdated[key].toString()=="") {
                qDebug() << "QString value:" << strValue;
                lastUpdated[key] = QDateTime::currentDateTime().addSecs(-10);
                qDebug() << "Updated lastUpdated[" << QString::fromStdString(key) << "] to:" << lastUpdated[key].toString();
                return;
            }else if(strValue=="inf"){
                qDebug() << "Updated lastUpdated[" << QString::fromStdString(key) << "] to:" << lastUpdated[key].toString();
                return;
            }
        }
        if (std::holds_alternative<double>(value)) {
            double dblValue = std::get<double>(value);
            //qDebug() << "Double value:" << dblValue;
            if (dblValue == INFINITY&&lastUpdated[key].toString()=="") {
                qDebug() << "Double value:" << dblValue;
                lastUpdated[key] = QDateTime::currentDateTime().addSecs(-10);
                qDebug() << "Updated lastUpdated[" << QString::fromStdString(key) << "] to:" << lastUpdated[key].toString();
                return;
            }else if(dblValue == INFINITY){
                qDebug() << "Double value:" << dblValue;
                lastUpdated[key] = lastUpdated[key].addSecs(-5);
                qDebug() << "Updated lastUpdated[" << QString::fromStdString(key) << "] to:" << lastUpdated[key].toString();
                return;
            }
        }
        if (it != updater.end()) {
            it->second(value);  // Update the member
            lastUpdated[key] = QDateTime::currentDateTime();  // Update timestamp
        } else {
            std::cerr << "Key not found: " << key << "\n";
        }
    }

    QDateTime getUpdateTime(const std::string& key) const {
        auto it = lastUpdated.find(key);
        if (it != lastUpdated.end()) {
            return it->second;
        } else {
            return QDateTime(); // Returns an invalid QDateTime if key is not found
        }
    }

    void printLastUpdatedTimes() const {
        for (const auto& [key, time] : lastUpdated) {
            std::cout << key << ": " << time.toString().toStdString() << "\n";
        }
    }

};



class read : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit read(QObject *parent = nullptr, MainWindow *mainWindow = nullptr);

    // Метод для чтения данных
    void file_reading();
    void updateParcingFile(QString fileName);

    QTime clock_time;
    // Вектор для хранения всех данных
    ResultStructure data_info;
    // Реализация обязательных методов
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        return QModelIndex(); // Пустая реализация
    }

    QModelIndex parent(const QModelIndex &child) const override {
        return QModelIndex(); // Пустая реализация
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return 0; // Пустая реализация
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        return 0; // Пустая реализация
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        return QVariant(); // Пустая реализация
    }
private:
    // Метод для обработки команды PSTM
    void PSTM_reading(QStringList list_of_param);
    void NMEA_reading(QStringList list_of_param);
    void Lat_Log(std::string key, double Coord);
    void Checksum_reading_STR(QString str,std::string key);
    void Checksum_reading_Double(QString str,std::string key);
    MainWindow *mainWindow;
    QString path_to_parcing_file;
    bool change_time_new_file;

};

#endif // READ_H
