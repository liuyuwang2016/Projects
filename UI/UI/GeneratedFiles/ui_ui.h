/********************************************************************************
** Form generated from reading UI file 'ui.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UI_H
#define UI_UI_H

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

class Ui_UIClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *UIClass)
    {
        if (UIClass->objectName().isEmpty())
            UIClass->setObjectName(QStringLiteral("UIClass"));
        UIClass->resize(600, 400);
        menuBar = new QMenuBar(UIClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        UIClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(UIClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        UIClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(UIClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        UIClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(UIClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        UIClass->setStatusBar(statusBar);

        retranslateUi(UIClass);

        QMetaObject::connectSlotsByName(UIClass);
    } // setupUi

    void retranslateUi(QMainWindow *UIClass)
    {
        UIClass->setWindowTitle(QApplication::translate("UIClass", "UI", 0));
    } // retranslateUi

};

namespace Ui {
    class UIClass: public Ui_UIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UI_H
