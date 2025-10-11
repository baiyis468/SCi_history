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
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TloginDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLabel *label;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *editName;
    QLineEdit *Editpassword;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnOK;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *btnCancel;

    void setupUi(QDialog *TloginDialog)
    {
        if (TloginDialog->objectName().isEmpty())
            TloginDialog->setObjectName("TloginDialog");
        TloginDialog->resize(277, 253);
        verticalLayout = new QVBoxLayout(TloginDialog);
        verticalLayout->setObjectName("verticalLayout");
        groupBox = new QGroupBox(TloginDialog);
        groupBox->setObjectName("groupBox");
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName("gridLayout");
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 1, 1, 2);

        editName = new QLineEdit(groupBox);
        editName->setObjectName("editName");

        gridLayout->addWidget(editName, 0, 4, 1, 1);

        Editpassword = new QLineEdit(groupBox);
        Editpassword->setObjectName("Editpassword");

        gridLayout->addWidget(Editpassword, 1, 3, 1, 2);

        horizontalSpacer = new QSpacerItem(42, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 2, 1, 1);


        verticalLayout->addWidget(groupBox);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        btnOK = new QPushButton(TloginDialog);
        btnOK->setObjectName("btnOK");

        horizontalLayout->addWidget(btnOK);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        btnCancel = new QPushButton(TloginDialog);
        btnCancel->setObjectName("btnCancel");

        horizontalLayout->addWidget(btnCancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(TloginDialog);
        QObject::connect(btnCancel, &QPushButton::clicked, TloginDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(TloginDialog);
    } // setupUi

    void retranslateUi(QDialog *TloginDialog)
    {
        TloginDialog->setWindowTitle(QCoreApplication::translate("TloginDialog", "Dialog", nullptr));
        groupBox->setTitle(QString());
        label_2->setText(QCoreApplication::translate("TloginDialog", "\345\257\206\347\240\201", nullptr));
        label->setText(QCoreApplication::translate("TloginDialog", "\347\224\250\346\210\267ID", nullptr));
        btnOK->setText(QCoreApplication::translate("TloginDialog", "\347\241\256\345\256\232", nullptr));
        btnCancel->setText(QCoreApplication::translate("TloginDialog", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TloginDialog: public Ui_TloginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TLOGINDIALOG_H
