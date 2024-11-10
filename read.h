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

    std::unordered_map<std::string, QDateTime> lastUpdated;

    using ValueType = std::variant<QString, double>;

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
    }

    void updateValue(const std::string& key, ValueType value) {
        auto it = updater.find(key);
        //std::visit([](auto&& arg) { qDebug() << "Type:" << typeid(arg).name() << "Value:" << arg; }, value);
        if (std::holds_alternative<QString>(value)) {
            QString strValue = std::get<QString>(value);
            //qDebug() << "QString value:" << strValue;
            if (strValue.isEmpty() &&lastUpdated[key].toString()=="") {
                qDebug() << "QString value:" << strValue;
                lastUpdated[key] = QDateTime::currentDateTime().addSecs(-10);
                qDebug() << "Updated lastUpdated[" << QString::fromStdString(key) << "] to:" << lastUpdated[key].toString();
                return;
            }else if(strValue.isEmpty()){
                lastUpdated[key] = lastUpdated[key].addSecs(-5);
                qDebug() << "Updated lastUpdated[" << QString::fromStdString(key) << "] to:" << lastUpdated[key].toString();
                return;
            }
        }
        if (std::holds_alternative<double>(value)) {
            double dblValue = std::get<double>(value);
            //qDebug() << "Double value:" << dblValue;
            if (dblValue == 0.0&&lastUpdated[key].toString()=="") {
                qDebug() << "Double value:" << dblValue;
                lastUpdated[key] = QDateTime::currentDateTime().addSecs(-10);
                qDebug() << "Updated lastUpdated[" << QString::fromStdString(key) << "] to:" << lastUpdated[key].toString();
                return;
            }else if(dblValue == 0.0){
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
    MainWindow *mainWindow;
    QString path_to_parcing_file;

};

#endif // READ_H
