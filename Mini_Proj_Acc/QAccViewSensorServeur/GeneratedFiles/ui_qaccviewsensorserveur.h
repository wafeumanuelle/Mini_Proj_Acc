/********************************************************************************
** Form generated from reading UI file 'qaccviewsensorserveur.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QACCVIEWSENSORSERVEUR_H
#define UI_QACCVIEWSENSORSERVEUR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QAccViewSensorServeurClass
{
public:
    QWidget *centralWidget;
    QListWidget *m_msg;
    QPushButton *m_send;
    QPushButton *m_kill_all;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QAccViewSensorServeurClass)
    {
        if (QAccViewSensorServeurClass->objectName().isEmpty())
            QAccViewSensorServeurClass->setObjectName(QString::fromUtf8("QAccViewSensorServeurClass"));
        QAccViewSensorServeurClass->resize(756, 456);
        centralWidget = new QWidget(QAccViewSensorServeurClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        m_msg = new QListWidget(centralWidget);
        m_msg->setObjectName(QString::fromUtf8("m_msg"));
        m_msg->setGeometry(QRect(0, 0, 601, 371));
        m_msg->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        m_send = new QPushButton(centralWidget);
        m_send->setObjectName(QString::fromUtf8("m_send"));
        m_send->setGeometry(QRect(620, 10, 112, 34));
        m_kill_all = new QPushButton(centralWidget);
        m_kill_all->setObjectName(QString::fromUtf8("m_kill_all"));
        m_kill_all->setGeometry(QRect(620, 70, 112, 34));
        QAccViewSensorServeurClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(QAccViewSensorServeurClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 756, 31));
        QAccViewSensorServeurClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QAccViewSensorServeurClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        QAccViewSensorServeurClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(QAccViewSensorServeurClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        QAccViewSensorServeurClass->setStatusBar(statusBar);

        retranslateUi(QAccViewSensorServeurClass);

        QMetaObject::connectSlotsByName(QAccViewSensorServeurClass);
    } // setupUi

    void retranslateUi(QMainWindow *QAccViewSensorServeurClass)
    {
        QAccViewSensorServeurClass->setWindowTitle(QApplication::translate("QAccViewSensorServeurClass", "QAccViewSensorServeur", 0, QApplication::UnicodeUTF8));
        m_send->setText(QApplication::translate("QAccViewSensorServeurClass", "Send", 0, QApplication::UnicodeUTF8));
        m_kill_all->setText(QApplication::translate("QAccViewSensorServeurClass", "Deconnect All", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QAccViewSensorServeurClass: public Ui_QAccViewSensorServeurClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QACCVIEWSENSORSERVEUR_H
