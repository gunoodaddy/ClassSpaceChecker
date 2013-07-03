#ifndef CLASSSPACECHECKER_H
#define CLASSSPACECHECKER_H

#include <QtGui/QMainWindow>
#include <QtGui>
#include "ui_classspacechecker.h"

#define VERSION_TEXT	"1.0.0"

#define AUTHOR_TEXT		"gunoodaddy"
#define PROGRAM_TEXT	"Jar Class Space Checker"


class ClassFileContext 
{
public:
	QString className;
	QString filePath;
	QString originalName;
	QString fullClassNameForKey;
	long fileSize;
};


class ClassSpaceChecker : public QMainWindow
{
	Q_OBJECT

public:
	ClassSpaceChecker(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ClassSpaceChecker();

public slots:
	void onClickedJarFile();
	void onClickedMapFile();
	void onCheckButtonClicked();
	void onClickedByUncryptName();
	void onChangedSearchText(QString text);
	void onResultItemSelectionChanged();
	void onResultCellDoubleClicked(int row, int column);
	bool eventFilter(QObject *object, QEvent *evt);

private:
	void changeResultHeader();
	bool loadJarFile(const QString & jarPath);
	bool loadMapFile(const QString & mapPath);
	void collectClassFile();
	void searchClass(bool useUncryptName, const QString & searchText);
	void removeAll();
	void openClassFile(const QString jarPath, const ClassFileContext *ctx);

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

	QVariant getSetting(const QString &key)
	{
		QString iniFile = qApp->applicationDirPath() + QDir::separator() + "setting.ini";
		QSettings settings( iniFile, QSettings::IniFormat );
		settings.beginGroup("Setting");
		QVariant value = settings.value(key);
		settings.endGroup();
		return value;
	}

	void setSetting(const QString &key, QVariant value)
	{
		QString iniFile = qApp->applicationDirPath() + QDir::separator() + "setting.ini";
		QSettings settings( iniFile, QSettings::IniFormat );
		settings.beginGroup("Setting");
		settings.setValue(key, value);
		settings.endGroup();
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
private:
	Ui::ClassSpaceCheckerClass ui;

private:
	QString currentJarPath_;
	QString currentMapPath_;
	QList<ClassFileContext*> classList_;
	QMap<QString, QString> proguardMap_VK_;
	QString prevTotalResultStr_;
	unsigned long prevJdProcessId_;
};

#endif // CLASSSPACECHECKER_H
