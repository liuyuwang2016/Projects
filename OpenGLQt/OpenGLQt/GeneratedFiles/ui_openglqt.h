/********************************************************************************
** Form generated from reading UI file 'openglqt.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPENGLQT_H
#define UI_OPENGLQT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "glwidget.h"

QT_BEGIN_NAMESPACE

class Ui_OpenGLQtClass
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QPushButton *pushButton;
    GLWidget *GLWidget;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *OpenGLQtClass)
    {
        if (OpenGLQtClass->objectName().isEmpty())
            OpenGLQtClass->setObjectName(QStringLiteral("OpenGLQtClass"));
        OpenGLQtClass->resize(800, 600);
        OpenGLQtClass->setMinimumSize(QSize(800, 600));
        centralWidget = new QWidget(OpenGLQtClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        verticalLayout_2->addLayout(verticalLayout);

        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        verticalLayout_2->addWidget(pushButton);


        horizontalLayout->addLayout(verticalLayout_2);

        GLWidget = new GLWidget(centralWidget);
        GLWidget->setObjectName(QStringLiteral("GLWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(GLWidget->sizePolicy().hasHeightForWidth());
        GLWidget->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(GLWidget);

        OpenGLQtClass->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(OpenGLQtClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        OpenGLQtClass->addToolBar(Qt::TopToolBarArea, mainToolBar);

        retranslateUi(OpenGLQtClass);
        QObject::connect(pushButton, SIGNAL(released()), OpenGLQtClass, SLOT(close()));

        QMetaObject::connectSlotsByName(OpenGLQtClass);
    } // setupUi

    void retranslateUi(QMainWindow *OpenGLQtClass)
    {
        OpenGLQtClass->setWindowTitle(QApplication::translate("OpenGLQtClass", "Qt +OpenGL", 0));
        pushButton->setText(QApplication::translate("OpenGLQtClass", "Quit", 0));
    } // retranslateUi

};

namespace Ui {
    class OpenGLQtClass: public Ui_OpenGLQtClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENGLQT_H
