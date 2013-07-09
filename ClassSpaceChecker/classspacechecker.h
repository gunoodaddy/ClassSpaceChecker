#ifndef CLASSSPACECHECKER_H
#define CLASSSPACECHECKER_H

#include <QtGui/QMainWindow>
#include <QtGui>
#include "ui_classspacechecker.h"
#include <atlbase.h>

#define VERSION_TEXT	"1.2.0"

#define AUTHOR_TEXT		"gunoodaddy"
#define PROGRAM_TEXT	"Java Class Analysis"


class ClassFileContext 
{
public:
	ClassFileContext() : fileSize(0), methodCount(0), referencedCount(0)
	{
	}

	QString className;
	QString filePath;
	QString originalName;
	QString fullClassNameForKey;
	long fileSize;
	int methodCount;
	int referencedCount;
	QByteArray decompiledBuffer;
	QSet<QString> classReferencedList;
};

class UniqueClassContext 
{
public:
	int classCount;
	long fileSize;
	QString uniqueClassName;
};

class PackageContext 
{
public:
	int classCount;
	int uniqueClassCount;
	long fileSize;
	QSet<QString> uniqueClassNameSet;
	QString packageName;
};


class ClassSpaceChecker : public QMainWindow
{
	Q_OBJECT

public:
	ClassSpaceChecker(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ClassSpaceChecker();

public slots:
	void onJarFileCurrentIndexChanged(int index);
	void onClickedIgnoreInnerClass();
	void onClickedExportCSV();
	void onClickedJarFile();
	void onClickedMapFile();
	void onCheckButtonClicked();
	void onClickedByUncryptName();
	void onChangedSearchText(QString text);
	void onChangedSearchClassName(QString text);
	void onResultItemSelectionChanged();
	void onResultCellDoubleClicked(int row, int column);
	void onPackageReportCellDoubleClicked(int row, int column);
	void onPackageReportItemSelectionChanged();
	void onUniqueClassReportItemSelectionChanged();
	void onTabCurrentChanged(int index);
	void onJarFileEditTextChanged(QString text);
	void onClickedDelete();
	bool eventFilter(QObject *object, QEvent *evt);

private:
	void buildStatusBar();
	void installStatusProgressBar(int maxValue);
	void uninstallStatusProgressBar();
	void setStatusProgressValue(int pos);
	QByteArray decompileClassAndReadFile(const QString &classFilePath);
	void checkAndJarFilePreset(const QString &jarPath);
	void saveCurrentPreset();
	void loadPreset(const QString &jarPath);
	void loadPresetList(const QString &selectPresetId);
	void changeResultHeader();
	bool loadJarFile(const QString & jarPath);
	bool loadMapFile(const QString & mapPath);
	void collectData();
	void searchClassInternal(const QString & searchText, bool classNameMode, bool useUncryptName, bool ignoreInnerClass);
	void searchClassByName(bool useUncryptName, bool ignoreInnerClass, const QString & searchText);
	void searchClassByText(const QString & searchText);
	void analysisPackageReport();
	void analysisUniqueClassReport();
	void removeAll();
	void openClassFile(const QString jarPath, const ClassFileContext *ctx);
	void writeToCSVFile(const QTableWidget *tableWidget, const QString & outputPath);
	unsigned long runProgram(const QString &theUri, const QString &param, bool silentMode = false, bool waitExit = false);

	void updateWindowTitle( void )
	{
		// Title
		QString newTitle = PROGRAM_TEXT;
		newTitle += " Ver ";
		newTitle += VERSION_TEXT;
		newTitle += " by ";
		newTitle += AUTHOR_TEXT;

		setWindowTitle( newTitle );
	}

	inline QString generateFileTempPath()
	{
		QString res = qApp->applicationDirPath();
		res += QDir::separator();
		res += "Temp";
		res += QDir::separator();

		QDir dir( res );
		if ( !dir.exists() )
			dir.mkpath( res );
		return QDir::toNativeSeparators(res);
	}

	QString numberDot(const QString & src)
	{
		QString number = src;

		int i = number.lastIndexOf('.');
		if(i < 0)
			i = number.length();

		i -= 3;
		while(i > 0)
		{
			number.insert(i, ',');
			i -= 3;
		}
		return number;
	}

	std::string toMBCS(const QString &src)
	{
		USES_CONVERSION;
		std::wstring wstr = src.toStdWString();
		return W2A(wstr.c_str());
	}

private:
	Ui::ClassSpaceCheckerClass ui;

private:
	QString currentJarPath_;
	QString currentMapPath_;
	QList<ClassFileContext*> classList_;
	QMap<QString, UniqueClassContext*> uniqueClassMap_;
	QMap<QString, PackageContext*> packageMap_;
	QMap<QString, QString> proguardMap_VK_;
	QString prevTotalResultStr_;
	unsigned long prevJdProcessId_;
	bool initJarFileComboFlag_;
	QProgressBar *progressBar_;
};

#endif // CLASSSPACECHECKER_H
