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
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtDropClass
{
public:
    QWidget *centralWidget;
    QLabel *m_label;
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
        m_label = new QLabel(centralWidget);
        m_label->setObjectName(QStringLiteral("m_label"));
        m_label->setGeometry(QRect(90, 40, 421, 21));
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
        m_label->setText(QApplication::translate("QtDropClass", "TextLabel", 0));
    } // retranslateUi

};

namespace Ui {
    class QtDropClass: public Ui_QtDropClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTDROP_H
