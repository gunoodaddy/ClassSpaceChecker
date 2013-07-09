/********************************************************************************
** Form generated from reading UI file 'classspacechecker.ui'
**
** Created: Wed Jul 10 02:18:14 2013
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
#include <QtGui/QComboBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
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
    QHBoxLayout *horizontalLayout_9;
    QComboBox *comboBox_JarFile;
    QPushButton *pushButton_Delete;
    QToolButton *toolButton_JarFile;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *lineEdit_MapFile;
    QToolButton *toolButton_MapFile;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButtonStart;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *lineEdit_Search;
    QCheckBox *checkBox_ByUncryptName;
    QLineEdit *lineEdit_SearchText;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBox_IgnoreInnerClass;
    QTabWidget *tabWidget;
    QWidget *tab;
    QHBoxLayout *horizontalLayout_6;
    QTableWidget *tableWidgetResult;
    QWidget *tab_2;
    QHBoxLayout *horizontalLayout_7;
    QTableWidget *tableWidgetPackageReport;
    QWidget *tab_3;
    QHBoxLayout *horizontalLayout_8;
    QTableWidget *tableWidgetInnerClassReport;
    QHBoxLayout *horizontalLayout_5;
    QLineEdit *lineEdit_Result;
    QPushButton *pushButtonExportCSV;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ClassSpaceCheckerClass)
    {
        if (ClassSpaceCheckerClass->objectName().isEmpty())
            ClassSpaceCheckerClass->setObjectName(QString::fromUtf8("ClassSpaceCheckerClass"));
        ClassSpaceCheckerClass->resize(1024, 768);
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
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        comboBox_JarFile = new QComboBox(centralWidget);
        comboBox_JarFile->setObjectName(QString::fromUtf8("comboBox_JarFile"));
        comboBox_JarFile->setAcceptDrops(true);
        comboBox_JarFile->setEditable(true);

        horizontalLayout_9->addWidget(comboBox_JarFile);

        pushButton_Delete = new QPushButton(centralWidget);
        pushButton_Delete->setObjectName(QString::fromUtf8("pushButton_Delete"));
        pushButton_Delete->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_9->addWidget(pushButton_Delete);

        toolButton_JarFile = new QToolButton(centralWidget);
        toolButton_JarFile->setObjectName(QString::fromUtf8("toolButton_JarFile"));

        horizontalLayout_9->addWidget(toolButton_JarFile);


        verticalLayout_2->addLayout(horizontalLayout_9);

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
        lineEdit_Search = new QLineEdit(centralWidget);
        lineEdit_Search->setObjectName(QString::fromUtf8("lineEdit_Search"));

        horizontalLayout_3->addWidget(lineEdit_Search);

        checkBox_ByUncryptName = new QCheckBox(centralWidget);
        checkBox_ByUncryptName->setObjectName(QString::fromUtf8("checkBox_ByUncryptName"));

        horizontalLayout_3->addWidget(checkBox_ByUncryptName);


        verticalLayout_2->addLayout(horizontalLayout_3);

        lineEdit_SearchText = new QLineEdit(centralWidget);
        lineEdit_SearchText->setObjectName(QString::fromUtf8("lineEdit_SearchText"));

        verticalLayout_2->addWidget(lineEdit_SearchText);

        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        checkBox_IgnoreInnerClass = new QCheckBox(groupBox);
        checkBox_IgnoreInnerClass->setObjectName(QString::fromUtf8("checkBox_IgnoreInnerClass"));

        verticalLayout->addWidget(checkBox_IgnoreInnerClass);

        tabWidget = new QTabWidget(groupBox);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        horizontalLayout_6 = new QHBoxLayout(tab);
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        tableWidgetResult = new QTableWidget(tab);
        tableWidgetResult->setObjectName(QString::fromUtf8("tableWidgetResult"));
        tableWidgetResult->setSelectionMode(QAbstractItemView::ExtendedSelection);
        tableWidgetResult->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidgetResult->setSortingEnabled(true);
        tableWidgetResult->setColumnCount(0);

        horizontalLayout_6->addWidget(tableWidgetResult);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        horizontalLayout_7 = new QHBoxLayout(tab_2);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        tableWidgetPackageReport = new QTableWidget(tab_2);
        tableWidgetPackageReport->setObjectName(QString::fromUtf8("tableWidgetPackageReport"));
        tableWidgetPackageReport->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidgetPackageReport->setSortingEnabled(true);

        horizontalLayout_7->addWidget(tableWidgetPackageReport);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        horizontalLayout_8 = new QHBoxLayout(tab_3);
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        tableWidgetInnerClassReport = new QTableWidget(tab_3);
        tableWidgetInnerClassReport->setObjectName(QString::fromUtf8("tableWidgetInnerClassReport"));
        tableWidgetInnerClassReport->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidgetInnerClassReport->setSortingEnabled(true);

        horizontalLayout_8->addWidget(tableWidgetInnerClassReport);

        tabWidget->addTab(tab_3, QString());

        verticalLayout->addWidget(tabWidget);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        lineEdit_Result = new QLineEdit(groupBox);
        lineEdit_Result->setObjectName(QString::fromUtf8("lineEdit_Result"));
        lineEdit_Result->setReadOnly(true);

        horizontalLayout_5->addWidget(lineEdit_Result);

        pushButtonExportCSV = new QPushButton(groupBox);
        pushButtonExportCSV->setObjectName(QString::fromUtf8("pushButtonExportCSV"));
        pushButtonExportCSV->setMinimumSize(QSize(100, 0));

        horizontalLayout_5->addWidget(pushButtonExportCSV);


        verticalLayout->addLayout(horizontalLayout_5);


        verticalLayout_2->addWidget(groupBox);


        verticalLayout_3->addLayout(verticalLayout_2);

        ClassSpaceCheckerClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(ClassSpaceCheckerClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        ClassSpaceCheckerClass->setStatusBar(statusBar);

        retranslateUi(ClassSpaceCheckerClass);
        QObject::connect(pushButtonStart, SIGNAL(clicked()), ClassSpaceCheckerClass, SLOT(onCheckButtonClicked()));
        QObject::connect(lineEdit_Search, SIGNAL(textChanged(QString)), ClassSpaceCheckerClass, SLOT(onChangedSearchClassName(QString)));
        QObject::connect(toolButton_JarFile, SIGNAL(clicked()), ClassSpaceCheckerClass, SLOT(onClickedJarFile()));
        QObject::connect(toolButton_MapFile, SIGNAL(clicked()), ClassSpaceCheckerClass, SLOT(onClickedMapFile()));
        QObject::connect(tableWidgetResult, SIGNAL(itemSelectionChanged()), ClassSpaceCheckerClass, SLOT(onResultItemSelectionChanged()));
        QObject::connect(checkBox_ByUncryptName, SIGNAL(clicked()), ClassSpaceCheckerClass, SLOT(onClickedByUncryptName()));
        QObject::connect(tableWidgetResult, SIGNAL(cellDoubleClicked(int,int)), ClassSpaceCheckerClass, SLOT(onResultCellDoubleClicked(int,int)));
        QObject::connect(pushButtonExportCSV, SIGNAL(clicked()), ClassSpaceCheckerClass, SLOT(onClickedExportCSV()));
        QObject::connect(tableWidgetPackageReport, SIGNAL(cellDoubleClicked(int,int)), ClassSpaceCheckerClass, SLOT(onPackageReportCellDoubleClicked(int,int)));
        QObject::connect(tableWidgetPackageReport, SIGNAL(itemSelectionChanged()), ClassSpaceCheckerClass, SLOT(onPackageReportItemSelectionChanged()));
        QObject::connect(tabWidget, SIGNAL(currentChanged(int)), ClassSpaceCheckerClass, SLOT(onTabCurrentChanged(int)));
        QObject::connect(tableWidgetInnerClassReport, SIGNAL(itemSelectionChanged()), ClassSpaceCheckerClass, SLOT(onInnerClassReportItemSelectionChanged()));
        QObject::connect(comboBox_JarFile, SIGNAL(currentIndexChanged(int)), ClassSpaceCheckerClass, SLOT(onJarFileCurrentIndexChanged(int)));
        QObject::connect(comboBox_JarFile, SIGNAL(editTextChanged(QString)), ClassSpaceCheckerClass, SLOT(onJarFileEditTextChanged(QString)));
        QObject::connect(pushButton_Delete, SIGNAL(clicked()), ClassSpaceCheckerClass, SLOT(onClickedDelete()));
        QObject::connect(lineEdit_SearchText, SIGNAL(textChanged(QString)), ClassSpaceCheckerClass, SLOT(onChangedSearchText(QString)));
        QObject::connect(checkBox_IgnoreInnerClass, SIGNAL(clicked()), ClassSpaceCheckerClass, SLOT(onClickedIgnoreInnerClass()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ClassSpaceCheckerClass);
    } // setupUi

    void retranslateUi(QMainWindow *ClassSpaceCheckerClass)
    {
        ClassSpaceCheckerClass->setWindowTitle(QApplication::translate("ClassSpaceCheckerClass", "Jar Class Space Checker by gunoodaddy", 0, QApplication::UnicodeUTF8));
        pushButton_Delete->setText(QApplication::translate("ClassSpaceCheckerClass", "Del", 0, QApplication::UnicodeUTF8));
        toolButton_JarFile->setText(QApplication::translate("ClassSpaceCheckerClass", "...", 0, QApplication::UnicodeUTF8));
        lineEdit_MapFile->setPlaceholderText(QApplication::translate("ClassSpaceCheckerClass", "Proguard Map File (Drag&Drop supported)", 0, QApplication::UnicodeUTF8));
        toolButton_MapFile->setText(QApplication::translate("ClassSpaceCheckerClass", "...", 0, QApplication::UnicodeUTF8));
        pushButtonStart->setText(QApplication::translate("ClassSpaceCheckerClass", "Analysis", 0, QApplication::UnicodeUTF8));
        lineEdit_Search->setPlaceholderText(QApplication::translate("ClassSpaceCheckerClass", "Search Class Name (Regular Expression supported)", 0, QApplication::UnicodeUTF8));
        checkBox_ByUncryptName->setText(QApplication::translate("ClassSpaceCheckerClass", "By Uncrypt Name", 0, QApplication::UnicodeUTF8));
        lineEdit_SearchText->setPlaceholderText(QApplication::translate("ClassSpaceCheckerClass", "Search Text", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("ClassSpaceCheckerClass", "Result", 0, QApplication::UnicodeUTF8));
        checkBox_IgnoreInnerClass->setText(QApplication::translate("ClassSpaceCheckerClass", "Ignore Inner Class", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("ClassSpaceCheckerClass", "File Report", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("ClassSpaceCheckerClass", "Package Report", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("ClassSpaceCheckerClass", "Inner Class Report", 0, QApplication::UnicodeUTF8));
        pushButtonExportCSV->setText(QApplication::translate("ClassSpaceCheckerClass", "Export this report to CSV", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ClassSpaceCheckerClass: public Ui_ClassSpaceCheckerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLASSSPACECHECKER_H
