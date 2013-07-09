#pragma once

#include <QtGui>

class CPresetData
{
public:
	static QString KEY_NAME;

public:
	CPresetData() { }
	CPresetData(QString id, QString iniFilePath) : id_(id), iniFilePath_(iniFilePath) { }
	~CPresetData(void)
	{
	}

	bool isEmpty() const { return id_.isEmpty(); }
	const QString &getId() const { return id_; }
	
	QString getKeyName()
	{
		return getValue(KEY_NAME).toString();
	}

	void setKeyName(const QString &keyName) 
	{
		setValue(KEY_NAME, keyName);
	}

	QVariant getValue(const QString &key)
	{
		if(isEmpty())
			return QVariant();

		QSettings settings( iniFilePath_, QSettings::IniFormat );
		settings.beginGroup(id_);
		QVariant value = settings.value(key);
		settings.endGroup();
		return value;
	}

	void setValue(const QString &key, QVariant value)
	{
		if(isEmpty())
			return;

		QSettings settings( iniFilePath_, QSettings::IniFormat );
		settings.beginGroup(id_);
		settings.setValue(key, value);
		settings.endGroup();
	}

private:
	QString id_;
	QString iniFilePath_;
};


class CSettingManager : public QObject
{
	Q_OBJECT

public:
	CSettingManager(void) : INI_FILE_("Setting.ini")
	{
		setLastPresetId(getLastPresetId());
	}

	~CSettingManager(void)
	{
	}

	void setIniPath(const QString &iniPath)
	{
		INI_FILE_ = iniPath;
	}

	int getPresetCount()
	{
		return getPresetList().size();
	}

	QList<CPresetData> getPresetList()
	{
		QList<CPresetData> res;
		int count = 0;
		QSettings settings(INI_FILE_, QSettings::IniFormat );
		QStringList list = settings.childGroups();
		for(int i = 0; i < list.size(); i++)
		{
			QString group = list.at(i);
			CPresetData data(group, INI_FILE_);
			res.append(data);
		}
		return res;
	}

	bool findPreset(const QString &id)
	{
		QList<CPresetData> list = getPresetList();
		for(int i = 0; i < list.size(); i++)
		{
			CPresetData preset = list.at(i);
			if(preset.getId() == id)
				return true;
		}
		return false;
	}

	CPresetData createPreset(const QString &keyName)
	{
		int max = getMaxPresetId() + 1;
		QString newId = QString::number(max);
		setMaxPresetId(max);

		QSettings settings( INI_FILE_, QSettings::IniFormat );
		settings.beginGroup(newId);
		settings.setValue(CPresetData::KEY_NAME, keyName);
		settings.endGroup();

		setLastPresetId(newId);

		return getPresetDataWithKeyName(keyName);
	}

	void setLastPresetId(const QString &id)
	{
		setValue("lastPresetId", id);
	}

	QString getLastPresetId()
	{
		return getValue("lastPresetId").toString();
	}

	void setValue(const QString &key, const QString &value)
	{
		QSettings settings( INI_FILE_, QSettings::IniFormat );
		settings.setValue(key, value);
	}

	void removeValue(const QString &key)
	{
		QSettings settings( INI_FILE_, QSettings::IniFormat );
		settings.remove(key);
	}

	QVariant getValue(const QString &key)
	{
		QSettings settings( INI_FILE_, QSettings::IniFormat );
		QString value = settings.value(key).toString();
		return value;
	}

	CPresetData getPresetDataWithKeyName(const QString &keyName)
	{
		QList<CPresetData> list = getPresetList();
		for(int i = 0; i < list.size(); i++)
		{
			CPresetData preset = list.at(i);
			if(preset.getKeyName() == keyName)
				return preset;
		}
		return CPresetData();
	}

	CPresetData getPresetDataWithId(const QString &id)
	{
		QList<CPresetData> list = getPresetList();
		for(int i = 0; i < list.size(); i++)
		{
			CPresetData preset = list.at(i);
			if(preset.getId() == id)
				return preset;
		}
		return CPresetData();
	}

private:
	int getMaxPresetId()
	{
		QSettings settings( INI_FILE_, QSettings::IniFormat );
		int maxId = settings.value("maxPresetId").toInt();
		return maxId;
	}

	void setMaxPresetId(int id)
	{
		QSettings settings( INI_FILE_, QSettings::IniFormat );
		settings.setValue("maxPresetId", id);
	}

private:
	QString INI_FILE_;
};
