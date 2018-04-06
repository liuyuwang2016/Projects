/********************************************************************************
** Form generated from reading UI file 'text8_3a.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXT8_3A_H
#define UI_TEXT8_3A_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Text8_3AClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineEdit;
    QPushButton *btnSelect;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnSend;
    QSpacerItem *horizontalSpacer_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Text8_3AClass)
    {
        if (Text8_3AClass->objectName().isEmpty())
            Text8_3AClass->setObjectName(QStringLiteral("Text8_3AClass"));
        Text8_3AClass->resize(318, 250);
        centralWidget = new QWidget(Text8_3AClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        lineEdit = new QLineEdit(centralWidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        horizontalLayout_2->addWidget(lineEdit);

        btnSelect = new QPushButton(centralWidget);
        btnSelect->setObjectName(QStringLiteral("btnSelect"));

        horizontalLayout_2->addWidget(btnSelect);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnSend = new QPushButton(centralWidget);
        btnSend->setObjectName(QStringLiteral("btnSend"));
        btnSend->setMinimumSize(QSize(60, 60));
        btnSend->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(btnSend);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);

        Text8_3AClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Text8_3AClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 318, 23));
        Text8_3AClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Text8_3AClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        Text8_3AClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Text8_3AClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Text8_3AClass->setStatusBar(statusBar);

        retranslateUi(Text8_3AClass);

        QMetaObject::connectSlotsByName(Text8_3AClass);
    } // setupUi

    void retranslateUi(QMainWindow *Text8_3AClass)
    {
        Text8_3AClass->setWindowTitle(QApplication::translate("Text8_3AClass", "Text8_3A", 0));
        btnSelect->setText(QApplication::translate("Text8_3AClass", "\346\265\217\350\247\210", 0));
        btnSend->setText(QApplication::translate("Text8_3AClass", "\344\274\240\351\200\201", 0));
    } // retranslateUi

};

namespace Ui {
    class Text8_3AClass: public Ui_Text8_3AClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXT8_3A_H
