#ifndef SATELLITE_WINDOW_H
#define SATELLITE_WINDOW_H

#include <QDialog>
#include <QtGui/qstandarditemmodel.h>

namespace Ui {
class Satellite_window;
}

class Satellite_window : public QDialog
{
    Q_OBJECT

public:
    explicit Satellite_window(QString index, QWidget *parent = nullptr);
    ~Satellite_window();

private:
    Ui::Satellite_window *ui;
    QStandardItemModel*model;
};

#endif // SATELLITE_WINDOW_H
