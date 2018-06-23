/********************************************************************************
** Form generated from reading UI file 'qtdrop.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTDROP_H
#define UI_QTDROP_H

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
#include "MyListWidget.h"

QT_BEGIN_NAMESPACE

class Ui_QtDropClass
{
public:
    QWidget *centralWidget;
    MyListWidget *listWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtDropClass)
    {
        if (QtDropClass->objectName().isEmpty())
            QtDropClass->setObjectName(QStringLiteral("QtDropClass"));
        QtDropClass->resize(600, 400);
        centralWidget = new QWidget(QtDropClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        listWidget = new MyListWidget(centralWidget);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setGeometry(QRect(35, 21, 531, 321));
        QtDropClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QtDropClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 600, 23));
        QtDropClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtDropClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QtDropClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QtDropClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QtDropClass->setStatusBar(statusBar);

        retranslateUi(QtDropClass);

        QMetaObject::connectSlotsByName(QtDropClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtDropClass)
    {
        QtDropClass->setWindowTitle(QApplication::translate("QtDropClass", "QtDrop", 0));
    } // retranslateUi

};

namespace Ui {
    class QtDropClass: public Ui_QtDropClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTDROP_H
