/********************************************************************************
** Form generated from reading UI file 'classspacechecker.ui'
**
** Created: Wed Jul 3 23:34:15 2013
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLASSSPACECHECKER_H
#define UI_CLASSSPACECHECKER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ClassSpaceCheckerClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit_JarFile;
    QToolButton *toolButton_JarFile;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineEdit_MapFile;
    QToolButton *toolButton_MapFile;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButtonStart;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *checkBox_ByCryptName;
    QLineEdit *lineEdit_Search;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QTableWidget *tableWidgetResult;
    QLineEdit *lineEdit_Result;

    void setupUi(QMainWindow *ClassSpaceCheckerClass)
    {
        if (ClassSpaceCheckerClass->objectName().isEmpty())
            ClassSpaceCheckerClass->setObjectName(QString::fromUtf8("ClassSpaceCheckerClass"));
        ClassSpaceCheckerClass->resize(437, 432);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ClassSpaceChecker/Resources/ClassSpaceChecker.ico"), QSize(), QIcon::Normal, QIcon::Off);
        ClassSpaceCheckerClass->setWindowIcon(icon);
        centralWidget = new QWidget(ClassSpaceCheckerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout_3 = new QVBoxLayout(centralWidget);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineEdit_JarFile = new QLineEdit(centralWidget);
        lineEdit_JarFile->setObjectName(QString::fromUtf8("lineEdit_JarFile"));

        horizontalLayout->addWidget(lineEdit_JarFile);

        toolButton_JarFile = new QToolButton(centralWidget);
        toolButton_JarFile->setObjectName(QString::fromUtf8("toolButton_JarFile"));

        horizontalLayout->addWidget(toolButton_JarFile);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        lineEdit_MapFile = new QLineEdit(centralWidget);
        lineEdit_MapFile->setObjectName(QString::fromUtf8("lineEdit_MapFile"));

        horizontalLayout_2->addWidget(lineEdit_MapFile);

        toolButton_MapFile = new QToolButton(centralWidget);
        toolButton_MapFile->setObjectName(QString::fromUtf8("toolButton_MapFile"));

        horizontalLayout_2->addWidget(toolButton_MapFile);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        pushButtonStart = new QPushButton(centralWidget);
        pushButtonStart->setObjectName(QString::fromUtf8("pushButtonStart"));
        pushButtonStart->setMinimumSize(QSize(200, 40));
        pushButtonStart->setIcon(icon);
        pushButtonStart->setIconSize(QSize(32, 32));

        horizontalLayout_4->addWidget(pushButtonStart);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        checkBox_ByCryptName = new QCheckBox(centralWidget);
        checkBox_ByCryptName->setObjectName(QString::fromUtf8("checkBox_ByCryptName"));

        horizontalLayout_3->addWidget(checkBox_ByCryptName);

        lineEdit_Search = new QLineEdit(centralWidget);
        lineEdit_Search->setObjectName(QString::fromUtf8("lineEdit_Search"));

        horizontalLayout_3->addWidget(lineEdit_Search);


        verticalLayout_2->addLayout(horizontalLayout_3);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tableWidgetResult = new QTableWidget(groupBox);
        tableWidgetResult->setObjectName(QString::fromUtf8("tableWidgetResult"));
        tableWidgetResult->setSelectionMode(QAbstractItemView::ExtendedSelection);
        tableWidgetResult->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidgetResult->setSortingEnabled(true);
        tableWidgetResult->setColumnCount(0);

        verticalLayout->addWidget(tableWidgetResult);

        lineEdit_Result = new QLineEdit(groupBox);
        lineEdit_Result->setObjectName(QString::fromUtf8("lineEdit_Result"));
        lineEdit_Result->setReadOnly(true);

        verticalLayout->addWidget(lineEdit_Result);


        verticalLayout_2->addWidget(groupBox);


        verticalLayout_3->addLayout(verticalLayout_2);

        ClassSpaceCheckerClass->setCentralWidget(centralWidget);

        retranslateUi(ClassSpaceCheckerClass);
        QObject::connect(pushButtonStart, SIGNAL(clicked()), ClassSpaceCheckerClass, SLOT(onCheckButtonClicked()));
        QObject::connect(lineEdit_Search, SIGNAL(textChanged(QString)), ClassSpaceCheckerClass, SLOT(onChangedSearchText(QString)));
        QObject::connect(toolButton_JarFile, SIGNAL(clicked()), ClassSpaceCheckerClass, SLOT(onClickedJarFile()));
        QObject::connect(toolButton_MapFile, SIGNAL(clicked()), ClassSpaceCheckerClass, SLOT(onClickedMapFile()));
        QObject::connect(tableWidgetResult, SIGNAL(itemSelectionChanged()), ClassSpaceCheckerClass, SLOT(onResultItemSelectionChanged()));
        QObject::connect(checkBox_ByCryptName, SIGNAL(clicked()), ClassSpaceCheckerClass, SLOT(onClickedByCryptName()));
        QObject::connect(tableWidgetResult, SIGNAL(cellDoubleClicked(int,int)), ClassSpaceCheckerClass, SLOT(onResultCellDoubleClicked(int,int)));

        QMetaObject::connectSlotsByName(ClassSpaceCheckerClass);
    } // setupUi

    void retranslateUi(QMainWindow *ClassSpaceCheckerClass)
    {
        ClassSpaceCheckerClass->setWindowTitle(QApplication::translate("ClassSpaceCheckerClass", "Jar Class Space Checker by gunoodaddy", 0, QApplication::UnicodeUTF8));
        lineEdit_JarFile->setPlaceholderText(QApplication::translate("ClassSpaceCheckerClass", "Jar File", 0, QApplication::UnicodeUTF8));
        toolButton_JarFile->setText(QApplication::translate("ClassSpaceCheckerClass", "...", 0, QApplication::UnicodeUTF8));
        lineEdit_MapFile->setPlaceholderText(QApplication::translate("ClassSpaceCheckerClass", "Proguard Map File", 0, QApplication::UnicodeUTF8));
        toolButton_MapFile->setText(QApplication::translate("ClassSpaceCheckerClass", "...", 0, QApplication::UnicodeUTF8));
        pushButtonStart->setText(QApplication::translate("ClassSpaceCheckerClass", "Analysis", 0, QApplication::UnicodeUTF8));
        checkBox_ByCryptName->setText(QApplication::translate("ClassSpaceCheckerClass", "By Crypt Name", 0, QApplication::UnicodeUTF8));
        lineEdit_Search->setPlaceholderText(QApplication::translate("ClassSpaceCheckerClass", "Search Class Name", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ClassSpaceCheckerClass", "Result", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ClassSpaceCheckerClass: public Ui_ClassSpaceCheckerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLASSSPACECHECKER_H
