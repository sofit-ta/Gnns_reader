/********************************************************************************
** Form generated from reading UI file 'satellite_window.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SATELLITE_WINDOW_H
#define UI_SATELLITE_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Satellite_window
{
public:
    QGridLayout *gridLayout;
    QTableView *tableView;
    QVBoxLayout *verticalLayout;
    QDialogButtonBox *buttonBox;
    QLabel *label;

    void setupUi(QDialog *Satellite_window)
    {
        if (Satellite_window->objectName().isEmpty())
            Satellite_window->setObjectName("Satellite_window");
        Satellite_window->resize(516, 298);
        gridLayout = new QGridLayout(Satellite_window);
        gridLayout->setObjectName("gridLayout");
        tableView = new QTableView(Satellite_window);
        tableView->setObjectName("tableView");

        gridLayout->addWidget(tableView, 0, 0, 1, 1);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        buttonBox = new QDialogButtonBox(Satellite_window);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        verticalLayout->addWidget(buttonBox);

        label = new QLabel(Satellite_window);
        label->setObjectName("label");

        verticalLayout->addWidget(label);


        gridLayout->addLayout(verticalLayout, 0, 1, 1, 1);


        retranslateUi(Satellite_window);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, Satellite_window, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, Satellite_window, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(Satellite_window);
    } // setupUi

    void retranslateUi(QDialog *Satellite_window)
    {
        Satellite_window->setWindowTitle(QCoreApplication::translate("Satellite_window", "\320\230\320\275\321\204\320\276\321\200\320\274\320\260\321\206\320\270\321\217 \320\276 \321\201\320\277\321\203\321\202\320\275\320\270\320\272\320\265", nullptr));
        label->setText(QCoreApplication::translate("Satellite_window", "\320\241\320\277\321\203\321\202\320\275\320\270\320\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Satellite_window: public Ui_Satellite_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SATELLITE_WINDOW_H
