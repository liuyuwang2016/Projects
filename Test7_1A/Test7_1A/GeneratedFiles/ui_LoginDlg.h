/********************************************************************************
** Form generated from reading UI file 'LoginDlg.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDLG_H
#define UI_LOGINDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LoginDlg
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *textUser;
    QHBoxLayout *horizontalLayout;
    QLabel *label_2;
    QLineEdit *textPasswd;
    QFrame *line;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnOk;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *LoginDlg)
    {
        if (LoginDlg->objectName().isEmpty())
            LoginDlg->setObjectName(QStringLiteral("LoginDlg"));
        LoginDlg->resize(373, 263);
        verticalLayout = new QVBoxLayout(LoginDlg);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(LoginDlg);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(40, 0));

        horizontalLayout_2->addWidget(label);

        textUser = new QLineEdit(LoginDlg);
        textUser->setObjectName(QStringLiteral("textUser"));
        textUser->setMinimumSize(QSize(60, 0));
        textUser->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_2->addWidget(textUser);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_2 = new QLabel(LoginDlg);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMinimumSize(QSize(40, 0));

        horizontalLayout->addWidget(label_2);

        textPasswd = new QLineEdit(LoginDlg);
        textPasswd->setObjectName(QStringLiteral("textPasswd"));
        textPasswd->setMinimumSize(QSize(60, 0));
        textPasswd->setMaximumSize(QSize(16777215, 16777215));
        textPasswd->setEchoMode(QLineEdit::Password);

        horizontalLayout->addWidget(textPasswd);


        verticalLayout->addLayout(horizontalLayout);

        line = new QFrame(LoginDlg);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        btnOk = new QPushButton(LoginDlg);
        btnOk->setObjectName(QStringLiteral("btnOk"));

        horizontalLayout_3->addWidget(btnOk);

        btnCancel = new QPushButton(LoginDlg);
        btnCancel->setObjectName(QStringLiteral("btnCancel"));

        horizontalLayout_3->addWidget(btnCancel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_3);


        retranslateUi(LoginDlg);

        QMetaObject::connectSlotsByName(LoginDlg);
    } // setupUi

    void retranslateUi(QDialog *LoginDlg)
    {
        LoginDlg->setWindowTitle(QApplication::translate("LoginDlg", "LoginDlg", 0));
        label->setText(QApplication::translate("LoginDlg", "\347\224\250\346\210\267\345\220\215", 0));
        label_2->setText(QApplication::translate("LoginDlg", "\345\257\206\347\240\201", 0));
        btnOk->setText(QApplication::translate("LoginDlg", "\347\241\256\350\256\244", 0));
        btnCancel->setText(QApplication::translate("LoginDlg", "\345\217\226\346\266\210", 0));
    } // retranslateUi

};

namespace Ui {
    class LoginDlg: public Ui_LoginDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDLG_H
