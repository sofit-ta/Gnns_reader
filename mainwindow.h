#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "qcustomplot.h"
#include "read.h"
#include "satellite_window.h"
#include <QMainWindow>
#include <QtCore/qstringlistmodel.h>
#include <QtGui/qstandarditemmodel.h>
#include <QtWidgets/qstatusbar.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class read;  // Вперёд объявление класса read
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static bool button_is_pressed;
    void fill_the_table(bool first_time);
    void on_open_file_triggered();
    void plotSatelliteData();

private slots:

    void on_listView_clicked(const QModelIndex &index);

    void on_listView_2_clicked(const QModelIndex &index);

    void on_listView_doubleClicked(const QModelIndex &index);

    void on_listView_2_doubleClicked(const QModelIndex &index);

    void startReadingData();

    void stopReadingData();
    void updateTable(); // Добавляем слот для обновления таблицы
    QColor updateColor(const QString key, ResultStructure parced_data);
    void updateClock();
    QColor chooseSatColor(Sputnik satellite);

private:
    Ui::MainWindow *ui;
    QStringListModel *model;
    QStringListModel *model1;
    QStandardItemModel*model2;
    Satellite_window*sw;
    read *re;
    QTimer *clocks; // Объявляем указатель на QTimer
    QStringList list_names; // Добавляем list_names как член класса
    QCustomPlot* objectSatPlot;
};
#endif // MAINWINDOW_H
