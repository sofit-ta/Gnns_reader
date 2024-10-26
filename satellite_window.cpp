#include "satellite_window.h"
#include "ui_satellite_window.h"

Satellite_window::Satellite_window(QString index,QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Satellite_window)
{
    ui->setupUi(this);
    ui->label->setText("Спутник " + index);
    QStringList list_names = {"Угол возвышения (в градусах)","Азимут (в градусах)","Отношение неущего сигнала к шуму","Псевдодальность",
                              "Смещение несущей частоты","и тд"};
    QStringList list_results;
    if(index=="32"){
    list_results = {"62","096","30","15748178.41","30992.22","..."};
    }else if(index=="01"){
    list_results = {"62","096","30","15748178.41","30992.22","другой спутник"};
    }else{
    list_results = {"62","096","30","15748178.41","30992.22","еще другой спутник"};
    }

    model = new QStandardItemModel(list_names.length(), 2, this);
    for(int row = 0; row < list_names.length(); ++row){
        QStandardItem *constantItem = new QStandardItem(list_names.at(row));
        constantItem->setFlags(Qt::ItemIsEnabled); // Make the item read-only
        model->setItem(row,0,constantItem);

        QStandardItem *resultItem = new QStandardItem(list_results.at(row));
        resultItem->setFlags(Qt::ItemIsEnabled); // Make the item read-only
        model->setItem(row, 1, resultItem);
    }
    ui->tableView->setModel(model);
    // resize to text columns
    ui->tableView->resizeColumnsToContents();
}

Satellite_window::~Satellite_window()
{
    delete ui;
}
