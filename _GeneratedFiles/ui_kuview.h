/********************************************************************************
** Form generated from reading UI file 'kuview.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KUVIEW_H
#define UI_KUVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "kv_glwidget.h"

QT_BEGIN_NAMESPACE

class Ui_kuviewClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    Kv_glwidget *glw_main;
    QLineEdit *le_modeluri;
    QTableView *tv_modelinfo;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *kuviewClass)
    {
        if (kuviewClass->objectName().isEmpty())
            kuviewClass->setObjectName(QStringLiteral("kuviewClass"));
        kuviewClass->resize(1145, 881);
        centralWidget = new QWidget(kuviewClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        glw_main = new Kv_glwidget(centralWidget);
        glw_main->setObjectName(QStringLiteral("glw_main"));
        glw_main->setMinimumSize(QSize(640, 480));

        verticalLayout->addWidget(glw_main);

        le_modeluri = new QLineEdit(centralWidget);
        le_modeluri->setObjectName(QStringLiteral("le_modeluri"));

        verticalLayout->addWidget(le_modeluri);

        tv_modelinfo = new QTableView(centralWidget);
        tv_modelinfo->setObjectName(QStringLiteral("tv_modelinfo"));

        verticalLayout->addWidget(tv_modelinfo);

        kuviewClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(kuviewClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1145, 21));
        kuviewClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(kuviewClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        kuviewClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(kuviewClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        kuviewClass->setStatusBar(statusBar);

        retranslateUi(kuviewClass);

        QMetaObject::connectSlotsByName(kuviewClass);
    } // setupUi

    void retranslateUi(QMainWindow *kuviewClass)
    {
        kuviewClass->setWindowTitle(QApplication::translate("kuviewClass", "kuview", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class kuviewClass: public Ui_kuviewClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KUVIEW_H
