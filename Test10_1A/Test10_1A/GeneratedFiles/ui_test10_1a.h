/********************************************************************************
** Form generated from reading UI file 'test10_1a.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEST10_1A_H
#define UI_TEST10_1A_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Test10_1AClass
{
public:
    QAction *actionNew;
    QAction *actionSave;
    QAction *actionSettings;
    QAction *actionHelp;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *plainTextEdit;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;

    void setupUi(QMainWindow *Test10_1AClass)
    {
        if (Test10_1AClass->objectName().isEmpty())
            Test10_1AClass->setObjectName(QStringLiteral("Test10_1AClass"));
        Test10_1AClass->resize(607, 370);
        actionNew = new QAction(Test10_1AClass);
        actionNew->setObjectName(QStringLiteral("actionNew"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Test10_1A/Resources/new.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew->setIcon(icon);
        actionSave = new QAction(Test10_1AClass);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Test10_1A/Resources/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon1);
        actionSettings = new QAction(Test10_1AClass);
        actionSettings->setObjectName(QStringLiteral("actionSettings"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Test10_1A/Resources/setting.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSettings->setIcon(icon2);
        actionHelp = new QAction(Test10_1AClass);
        actionHelp->setObjectName(QStringLiteral("actionHelp"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/Test10_1A/Resources/help.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionHelp->setIcon(icon3);
        centralWidget = new QWidget(Test10_1AClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        plainTextEdit = new QPlainTextEdit(centralWidget);
        plainTextEdit->setObjectName(QStringLiteral("plainTextEdit"));

        verticalLayout->addWidget(plainTextEdit);

        Test10_1AClass->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(Test10_1AClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        Test10_1AClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Test10_1AClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Test10_1AClass->setStatusBar(statusBar);
        menuBar = new QMenuBar(Test10_1AClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 607, 23));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QStringLiteral("menu_2"));
        Test10_1AClass->setMenuBar(menuBar);

        mainToolBar->addAction(actionNew);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionSave);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionSettings);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionHelp);
        mainToolBar->addSeparator();
        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menu->addAction(actionNew);
        menu->addAction(actionSave);
        menu_2->addAction(actionHelp);
        menu_2->addAction(actionSettings);

        retranslateUi(Test10_1AClass);

        QMetaObject::connectSlotsByName(Test10_1AClass);
    } // setupUi

    void retranslateUi(QMainWindow *Test10_1AClass)
    {
        Test10_1AClass->setWindowTitle(QApplication::translate("Test10_1AClass", "Test10_1A", 0));
        actionNew->setText(QApplication::translate("Test10_1AClass", "\346\226\260\345\273\272", 0));
        actionSave->setText(QApplication::translate("Test10_1AClass", "\344\277\235\345\255\230", 0));
        actionSettings->setText(QApplication::translate("Test10_1AClass", "\350\256\276\347\275\256", 0));
        actionHelp->setText(QApplication::translate("Test10_1AClass", "\345\270\256\345\212\251", 0));
        menu->setTitle(QApplication::translate("Test10_1AClass", "\346\226\207\344\273\266", 0));
        menu_2->setTitle(QApplication::translate("Test10_1AClass", "\345\270\256\345\212\251", 0));
    } // retranslateUi

};

namespace Ui {
    class Test10_1AClass: public Ui_Test10_1AClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEST10_1A_H
