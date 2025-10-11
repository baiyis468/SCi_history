/********************************************************************************
** Form generated from reading UI file 'tlogindialog.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TLOGINDIALOG_H
#define UI_TLOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_TLoginDialog
{
public:
    QLineEdit *editName;
    QLineEdit *editPSWD;
    QLabel *labelName;
    QLabel *labelPSWD;
    QPushButton *btnOK;

    void setupUi(QDialog *TLoginDialog)
    {
        if (TLoginDialog->objectName().isEmpty())
            TLoginDialog->setObjectName("TLoginDialog");
        TLoginDialog->resize(371, 293);
        editName = new QLineEdit(TLoginDialog);
        editName->setObjectName("editName");
        editName->setGeometry(QRect(172, 50, 151, 23));
        editPSWD = new QLineEdit(TLoginDialog);
        editPSWD->setObjectName("editPSWD");
        editPSWD->setGeometry(QRect(172, 100, 151, 23));
        labelName = new QLabel(TLoginDialog);
        labelName->setObjectName("labelName");
        labelName->setGeometry(QRect(60, 50, 54, 16));
        labelPSWD = new QLabel(TLoginDialog);
        labelPSWD->setObjectName("labelPSWD");
        labelPSWD->setGeometry(QRect(60, 100, 54, 16));
        btnOK = new QPushButton(TLoginDialog);
        btnOK->setObjectName("btnOK");
        btnOK->setGeometry(QRect(100, 200, 161, 51));

        retranslateUi(TLoginDialog);

        QMetaObject::connectSlotsByName(TLoginDialog);
    } // setupUi

    void retranslateUi(QDialog *TLoginDialog)
    {
        TLoginDialog->setWindowTitle(QCoreApplication::translate("TLoginDialog", "Dialog", nullptr));
        labelName->setText(QCoreApplication::translate("TLoginDialog", "\350\264\246\345\217\267", nullptr));
        labelPSWD->setText(QCoreApplication::translate("TLoginDialog", "\345\257\206\347\240\201", nullptr));
        btnOK->setText(QCoreApplication::translate("TLoginDialog", "\347\231\273\345\275\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TLoginDialog: public Ui_TLoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TLOGINDIALOG_H
