/********************************************************************************
** Form generated from reading UI file 'test7_2a.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEST7_2A_H
#define UI_TEST7_2A_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_test7_2aClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnOpen;
    QPushButton *btnSave;
    QSpacerItem *horizontalSpacer;
    QPlainTextEdit *plainTextEdit;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *test7_2aClass)
    {
        if (test7_2aClass->objectName().isEmpty())
            test7_2aClass->setObjectName(QStringLiteral("test7_2aClass"));
        test7_2aClass->resize(600, 400);
        centralWidget = new QWidget(test7_2aClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        btnOpen = new QPushButton(centralWidget);
        btnOpen->setObjectName(QStringLiteral("btnOpen"));

        horizontalLayout->addWidget(btnOpen);

        btnSave = new QPushButton(centralWidget);
        btnSave->setObjectName(QStringLiteral("btnSave"));

        horizontalLayout->addWidget(btnSave);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        plainTextEdit = new QPlainTextEdit(centralWidget);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));

        verticalLayout->addWidget(plainTextEdit);

        test7_2aClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(test7_2aClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 23));
        test7_2aClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(test7_2aClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        test7_2aClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(test7_2aClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        test7_2aClass->setStatusBar(statusBar);

        retranslateUi(test7_2aClass);

        QMetaObject::connectSlotsByName(test7_2aClass);
    } // setupUi

    void retranslateUi(QMainWindow *test7_2aClass)
    {
        test7_2aClass->setWindowTitle(QApplication::translate("test7_2aClass", "test7_2a", 0));
        btnOpen->setText(QApplication::translate("test7_2aClass", "\346\211\223\345\274\200", 0));
        btnSave->setText(QApplication::translate("test7_2aClass", "\344\277\235\345\255\230", 0));
    } // retranslateUi

};

namespace Ui {
    class test7_2aClass: public Ui_test7_2aClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEST7_2A_H
