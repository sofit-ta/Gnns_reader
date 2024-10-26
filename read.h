#ifndef READ_H
#define READ_H



#include <QAbstractItemModel>
#include <QVector>
#include <QString>
#include <QDebug>

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
