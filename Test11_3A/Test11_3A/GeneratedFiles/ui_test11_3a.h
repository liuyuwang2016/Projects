/********************************************************************************
** Form generated from reading UI file 'test11_3a.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEST11_3A_H
#define UI_TEST11_3A_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Test11_3AClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Test11_3AClass)
    {
        if (Test11_3AClass->objectName().isEmpty())
            Test11_3AClass->setObjectName(QStringLiteral("Test11_3AClass"));
        Test11_3AClass->resize(600, 400);
        menuBar = new QMenuBar(Test11_3AClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        Test11_3AClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Test11_3AClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        Test11_3AClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(Test11_3AClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        Test11_3AClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(Test11_3AClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Test11_3AClass->setStatusBar(statusBar);

        retranslateUi(Test11_3AClass);

        QMetaObject::connectSlotsByName(Test11_3AClass);
    } // setupUi

    void retranslateUi(QMainWindow *Test11_3AClass)
    {
        Test11_3AClass->setWindowTitle(QApplication::translate("Test11_3AClass", "Test11_3A", 0));
    } // retranslateUi

};

namespace Ui {
    class Test11_3AClass: public Ui_Test11_3AClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEST11_3A_H
