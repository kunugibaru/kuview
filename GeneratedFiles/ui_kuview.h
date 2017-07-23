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
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QSpinBox>
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
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    Kv_glwidget *glw_main;
    QFormLayout *formLayout_2;
    QLineEdit *le_modeluri;
    QLabel *label_2;
    QLineEdit *le_basecolor;
    QLabel *label_4;
    QLineEdit *le_roughness;
    QLabel *label_5;
    QLineEdit *le_normal;
    QLabel *label_3;
    QLineEdit *le_metallic;
    QLabel *label_6;
    QTableView *tv_modelinfo;
    QPlainTextEdit *te_log;
    QFormLayout *formLayout;
    QLabel *lbl_intensity;
    QSpinBox *sb_intensity;
    QSpinBox *sb_ambient;
    QLabel *label;
    QPlainTextEdit *te_fs;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *kuviewClass)
    {
        if (kuviewClass->objectName().isEmpty())
            kuviewClass->setObjectName(QStringLiteral("kuviewClass"));
        kuviewClass->resize(1429, 881);
        centralWidget = new QWidget(kuviewClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout_3 = new QHBoxLayout(centralWidget);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        glw_main = new Kv_glwidget(centralWidget);
        glw_main->setObjectName(QStringLiteral("glw_main"));
        glw_main->setMinimumSize(QSize(640, 480));

        verticalLayout->addWidget(glw_main);

        formLayout_2 = new QFormLayout();
        formLayout_2->setSpacing(6);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        le_modeluri = new QLineEdit(centralWidget);
        le_modeluri->setObjectName(QStringLiteral("le_modeluri"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, le_modeluri);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_2);

        le_basecolor = new QLineEdit(centralWidget);
        le_basecolor->setObjectName(QStringLiteral("le_basecolor"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, le_basecolor);

        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_4);

        le_roughness = new QLineEdit(centralWidget);
        le_roughness->setObjectName(QStringLiteral("le_roughness"));

        formLayout_2->setWidget(4, QFormLayout::FieldRole, le_roughness);

        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, label_5);

        le_normal = new QLineEdit(centralWidget);
        le_normal->setObjectName(QStringLiteral("le_normal"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, le_normal);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, label_3);

        le_metallic = new QLineEdit(centralWidget);
        le_metallic->setObjectName(QStringLiteral("le_metallic"));

        formLayout_2->setWidget(5, QFormLayout::FieldRole, le_metallic);

        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout_2->setWidget(5, QFormLayout::LabelRole, label_6);


        verticalLayout->addLayout(formLayout_2);

        tv_modelinfo = new QTableView(centralWidget);
        tv_modelinfo->setObjectName(QStringLiteral("tv_modelinfo"));
        tv_modelinfo->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

        verticalLayout->addWidget(tv_modelinfo);

        te_log = new QPlainTextEdit(centralWidget);
        te_log->setObjectName(QStringLiteral("te_log"));
        te_log->setReadOnly(true);

        verticalLayout->addWidget(te_log);


        horizontalLayout_3->addLayout(verticalLayout);

        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        lbl_intensity = new QLabel(centralWidget);
        lbl_intensity->setObjectName(QStringLiteral("lbl_intensity"));

        formLayout->setWidget(0, QFormLayout::LabelRole, lbl_intensity);

        sb_intensity = new QSpinBox(centralWidget);
        sb_intensity->setObjectName(QStringLiteral("sb_intensity"));
        sb_intensity->setMaximum(10000);
        sb_intensity->setValue(30);

        formLayout->setWidget(0, QFormLayout::FieldRole, sb_intensity);

        sb_ambient = new QSpinBox(centralWidget);
        sb_ambient->setObjectName(QStringLiteral("sb_ambient"));
        sb_ambient->setMaximum(1000);
        sb_ambient->setValue(1);

        formLayout->setWidget(1, QFormLayout::FieldRole, sb_ambient);

        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label);


        horizontalLayout_3->addLayout(formLayout);

        te_fs = new QPlainTextEdit(centralWidget);
        te_fs->setObjectName(QStringLiteral("te_fs"));
        te_fs->setMinimumSize(QSize(600, 0));

        horizontalLayout_3->addWidget(te_fs);

        kuviewClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(kuviewClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1429, 21));
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
        label_2->setText(QApplication::translate("kuviewClass", "Model Path", Q_NULLPTR));
        label_4->setText(QApplication::translate("kuviewClass", "Basecolor Map", Q_NULLPTR));
        label_5->setText(QApplication::translate("kuviewClass", "Specular Map", Q_NULLPTR));
        label_3->setText(QApplication::translate("kuviewClass", "Normal Map", Q_NULLPTR));
        label_6->setText(QApplication::translate("kuviewClass", "Metallic Map", Q_NULLPTR));
        lbl_intensity->setText(QApplication::translate("kuviewClass", "Light Intensity", Q_NULLPTR));
        label->setText(QApplication::translate("kuviewClass", "Ambient(x0.01)", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class kuviewClass: public Ui_kuviewClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KUVIEW_H
