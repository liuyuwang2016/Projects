/********************************************************************************
** Form generated from reading UI file 'test7_1a.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEST7_1A_H
#define UI_TEST7_1A_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Test7_1AClass
{
public:
    QWidget *centralWidget;
    QPushButton *btnLogin;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Test7_1AClass)
    {
        if (Test7_1AClass->objectName().isEmpty())
            Test7_1AClass->setObjectName(QStringLiteral("Test7_1AClass"));
        Test7_1AClass->resize(600, 400);
        centralWidget = new QWidget(Test7_1AClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        btnLogin = new QPushButton(centralWidget);
        btnLogin->setObjectName(QStringLiteral("btnLogin"));
        btnLogin->setGeometry(QRect(120, 90, 75, 23));
        Test7_1AClass->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(Test7_1AClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        Test7_1AClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Test7_1AClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Test7_1AClass->setStatusBar(statusBar);

        retranslateUi(Test7_1AClass);

        QMetaObject::connectSlotsByName(Test7_1AClass);
    } // setupUi

    void retranslateUi(QMainWindow *Test7_1AClass)
    {
        Test7_1AClass->setWindowTitle(QApplication::translate("Test7_1AClass", "Test7_1A", 0));
        btnLogin->setText(QApplication::translate("Test7_1AClass", "\347\231\273\345\275\225", 0));
    } // retranslateUi

};

namespace Ui {
    class Test7_1AClass: public Ui_Test7_1AClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEST7_1A_H
