/********************************************************************************
** Form generated from reading UI file 'dispenser.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPENSER_H
#define UI_DISPENSER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DispenserClass
{
public:
    QWidget *centralWidget;
    QOpenGLWidget *openGLWidget;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_O;
    QMenu *menu_S;
    QMenu *menu_H;

    void setupUi(QMainWindow *DispenserClass)
    {
        if (DispenserClass->objectName().isEmpty())
            DispenserClass->setObjectName(QStringLiteral("DispenserClass"));
        DispenserClass->resize(1272, 901);
        centralWidget = new QWidget(DispenserClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        openGLWidget = new QOpenGLWidget(centralWidget);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setGeometry(QRect(10, 10, 1251, 851));
        DispenserClass->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(DispenserClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        DispenserClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(DispenserClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        DispenserClass->setStatusBar(statusBar);
        menuBar = new QMenuBar(DispenserClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1272, 23));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menu_O = new QMenu(menuBar);
        menu_O->setObjectName(QStringLiteral("menu_O"));
        menu_S = new QMenu(menuBar);
        menu_S->setObjectName(QStringLiteral("menu_S"));
        menu_H = new QMenu(menuBar);
        menu_H->setObjectName(QStringLiteral("menu_H"));
        DispenserClass->setMenuBar(menuBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_O->menuAction());
        menuBar->addAction(menu_S->menuAction());
        menuBar->addAction(menu_H->menuAction());

        retranslateUi(DispenserClass);

        QMetaObject::connectSlotsByName(DispenserClass);
    } // setupUi

    void retranslateUi(QMainWindow *DispenserClass)
    {
        DispenserClass->setWindowTitle(QApplication::translate("DispenserClass", "Dispenser", 0));
        menu->setTitle(QApplication::translate("DispenserClass", "\347\202\271\350\203\266\346\234\272\345\212\237\350\203\275 (D)", 0));
        menu_O->setTitle(QApplication::translate("DispenserClass", "\347\252\227\344\275\223 (O)", 0));
        menu_S->setTitle(QApplication::translate("DispenserClass", "\350\256\276\347\275\256 (S)", 0));
        menu_H->setTitle(QApplication::translate("DispenserClass", "\345\270\256\345\212\251 (H)", 0));
    } // retranslateUi

};

namespace Ui {
    class DispenserClass: public Ui_DispenserClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPENSER_H
