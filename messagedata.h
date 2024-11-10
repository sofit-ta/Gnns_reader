#ifndef MESSAGEDATA_H
#define MESSAGEDATA_H

#include <QtCore/qdatetime.h>
#include <QtCore/qobject.h>
#include <iostream>
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

    // Map to store member pointers by their key names
    std::unordered_map<std::string, std::function<void(double)>> updaterDouble;
    std::unordered_map<std::string, std::function<void(QString)>> updaterString;


    ResultStructure() {
        // Initialize the updater map for doubles
        updaterDouble["gps_time"] = [this](double value) { gps_time = value; };
        updaterDouble["Timestamp"] = [this](double value) { Timestamp = value; };
        updaterDouble["Lat"] = [this](double value) { Lat = value; };
        updaterDouble["Long"] = [this](double value) { Long = value; };
        updaterDouble["Alt"] = [this](double value) { Alt = value; };
        updaterDouble["VelN"] = [this](double value) { VelN = value; };
        updaterDouble["VelE"] = [this](double value) { VelE = value; };
        updaterDouble["VelV"] = [this](double value) { VelV = value; };
        updaterDouble["PCovN"] = [this](double value) { PCovN = value; };
        updaterDouble["PCovNE"] = [this](double value) { PCovNE = value; };
        updaterDouble["PCovNV"] = [this](double value) { PCovNV = value; };
        updaterDouble["PCovE"] = [this](double value) { PCovE = value; };
        updaterDouble["PCovEV"] = [this](double value) { PCovEV = value; };
        updaterDouble["PCovV"] = [this](double value) { PCovV = value; };
        updaterDouble["VCovN"] = [this](double value) { VCovN = value; };
        updaterDouble["VCovNE"] = [this](double value) { VCovNE = value; };
        updaterDouble["VCovNV"] = [this](double value) { VCovNV = value; };
        updaterDouble["VCovE"] = [this](double value) { VCovE = value; };
        updaterDouble["VCovEV"] = [this](double value) { VCovEV = value; };
        updaterDouble["VCovV"] = [this](double value) { VCovV = value; };

        // Initialize the updater map for strings
        updaterString["NS"] = [this](QString value) { NS = value; };
        updaterString["EW"] = [this](QString value) { EW = value; };
        updaterString["AltVal"] = [this](QString value) { AltVal = value; };
        updaterString["Checksum"] = [this](QString value) { Checksum = value; };
    }
    void updateValue(const std::string& key,QString value){
        if()
    }
    void updateValueDouble(const std::string& key, double value) {
        auto it = updaterDouble.find(key);
        if (key =="Lat"){
            value.mid(0, 2).toDouble() + value.mid(2).toDouble() / 60;
        }
        if (it != updaterDouble.end()) {
            it->second(value);  // Update the member
            lastUpdated[key] = QDateTime::currentDateTime();  // Update timestamp
        } else {
            std::cerr << "Key not found: " << key << "n";
        }
    }

    void updateValueString(const std::string& key, QString value) {
        auto it = updaterString.find(key);
        if (it != updaterString.end()) {
            it->second(value);  // Update the member
            lastUpdated[key] = QDateTime::currentDateTime();  // Update timestamp
        } else {
            std::cerr << "Key not found"<< "n";
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

class MessageData
{
public:

    ResultStructure data_info;
    MessageData();
};

#endif // MESSAGEDATA_H
