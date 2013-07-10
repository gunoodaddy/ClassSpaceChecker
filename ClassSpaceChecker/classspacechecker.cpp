#include "classspacechecker.h"
#include <QtGui>
#include <qt_windows.h>
#include "XZip/XUnZip.h"
#include "SettingManager.hpp"
#include "jclass/jclass.h"

CSettingManager gSettingManager;

ClassSpaceChecker::ClassSpaceChecker(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), prevJdProcessId_(0), initJarFileComboFlag_(false)
{
	gSettingManager.setIniPath(qApp->applicationDirPath() + QDir::separator() + "data.ini");

	ui.setupUi(this);
	ui.tableWidgetResult->horizontalHeader()->setResizeMode( QHeaderView::Interactive );

	ui.tableWidgetResult->setColumnCount(5);
	ui.tableWidgetResult->setHorizontalHeaderLabels(QString("Class Name;File Size;Uncrypted Name;Method Count;Referenced Count;").split(";"));  
	ui.tableWidgetResult->horizontalHeader()->setResizeMode( QHeaderView::Interactive );

	ui.tableWidgetPackageReport->setColumnCount(5);
	ui.tableWidgetPackageReport->setHorizontalHeaderLabels(QString("Package Name;All Class Count;Unique Count;Diff Count;File Size").split(";"));  
	ui.tableWidgetPackageReport->horizontalHeader()->setResizeMode( QHeaderView::Interactive );

	ui.tableWidgetInnerClassReport->setColumnCount(3);
	ui.tableWidgetInnerClassReport->setHorizontalHeaderLabels(QString("Class Name;Inner Class Count;File Size").split(";"));  
	ui.tableWidgetInnerClassReport->horizontalHeader()->setResizeMode( QHeaderView::Interactive );

	ui.comboBox_JarFile->lineEdit()->setPlaceholderText("Jar File (Drag&Drop supported)");
	ui.comboBox_JarFile->installEventFilter( this );
	ui.lineEdit_MapFile->setDragEnabled(true);
	ui.lineEdit_MapFile->installEventFilter( this );
	ui.tableWidgetResult->installEventFilter( this );

	ui.lineEdit_Result->setText("Press Analysis button first.");

	updateWindowTitle();

	loadPresetList(gSettingManager.getLastPresetId());

	loadPreset(gSettingManager.getLastPresetId());

	if(ui.comboBox_JarFile->currentText().isEmpty() && ui.lineEdit_MapFile->text().isEmpty())
	{
		// To show placeholder text at initial launch time
		ui.pushButtonStart->setFocus();
	}

	buildStatusBar();
}

ClassSpaceChecker::~ClassSpaceChecker()
{

}

void ClassSpaceChecker::buildStatusBar()
{
	progressBar_ = new QProgressBar();

	// 한번 넣었다 빼야지 프로그레스바의 사이즈가 도중에 변경되지 않는다.. 편법임.
	installStatusProgressBar(0);
	uninstallStatusProgressBar();
}

void ClassSpaceChecker::installStatusProgressBar(int maxValue)
{
	progressBar_->setRange(0, maxValue);
	progressBar_->setValue(0);
	progressBar_->show();

	ui.statusBar->addPermanentWidget(progressBar_, 1);
}

void ClassSpaceChecker::uninstallStatusProgressBar()
{
	ui.statusBar->removeWidget( progressBar_ );
}

void ClassSpaceChecker::setStatusProgressValue(int pos)
{
	progressBar_->setValue(pos);
}

bool ClassSpaceChecker::eventFilter(QObject *object, QEvent *evt)
{
	if(evt->type() == QEvent::FocusOut) 
	{
		if(object == ui.tableWidgetResult)
		{
			ui.lineEdit_Result->setText(prevTotalResultStr_);
		}
		return QMainWindow::eventFilter(object, evt);
	}

	if(object == ui.comboBox_JarFile 
		|| object == ui.lineEdit_MapFile
		) 
	{
		if(evt->type() == QEvent::DragEnter)
		{
			QDragEnterEvent *e = (QDragEnterEvent*)evt;
			const QMimeData *mimeData = e->mimeData();
			if (mimeData->hasUrls())
			{
				int cnt = 0;
				QList<QUrl> urlList = mimeData->urls();
				for (int i = 0; i < urlList.size(); ++i)
				{
					QString path = urlList.at(i).toLocalFile();
					if( !QFileInfo(path).isFile() )
						continue;

					if(path.indexOf(".jar", 0, Qt::CaseInsensitive) < 0 
						&& path.indexOf(".zip", 0, Qt::CaseInsensitive) < 0
						&& path.indexOf(".txt", 0, Qt::CaseInsensitive) < 0
						)
						continue;

					if(urlList.size() == 1) 
					{
						if(object == ui.comboBox_JarFile)
						{
							if(path.indexOf(".jar", 0, Qt::CaseInsensitive) < 0 
								&& path.indexOf(".zip", 0, Qt::CaseInsensitive) < 0
								)
								continue;
						}
						else if(object == ui.lineEdit_MapFile)
						{
							if(path.indexOf(".txt", 0, Qt::CaseInsensitive) < 0)
								continue;
						}
					}

					cnt++;
				}
				if( cnt > 0 )
					e->acceptProposedAction();
			}
			qDebug() << evt << mimeData->text();
		}
		else if(evt->type() == QEvent::DragLeave)
		{
			QDragLeaveEvent *e = (QDragLeaveEvent*)evt;
			e->accept();
		}
		else if(evt->type() == QEvent::DragMove)
		{
			QDragMoveEvent *e = (QDragMoveEvent*)evt;
			e->acceptProposedAction();
		}
		else if(evt->type() == QEvent::Drop)
		{
			QDropEvent *e = (QDropEvent*)evt;
			const QMimeData* mimeData = e->mimeData();

			if (mimeData->hasUrls())
			{
				QList<QUrl> urlList = mimeData->urls();

				for (int i = 0; i < urlList.size(); ++i)
				{
					QString path = urlList.at(i).toLocalFile();

					if(path.indexOf(".txt", 0, Qt::CaseInsensitive) >= 0)
						ui.lineEdit_MapFile->setText(path);
					if(path.indexOf(".jar", 0, Qt::CaseInsensitive) >= 0 || path.indexOf(".zip", 0, Qt::CaseInsensitive) >= 0)
					{
						checkAndJarFilePreset(path);
					}
				}
			}
		}
	}
	return QMainWindow::eventFilter(object, evt);
}

void ClassSpaceChecker::checkAndJarFilePreset(const QString &jarPath)
{
	CPresetData preset = gSettingManager.getPresetDataWithKeyName(jarPath);

	if(preset.isEmpty()) 
	{
		ui.comboBox_JarFile->setEditText(jarPath);
		ui.lineEdit_MapFile->setText("");
	}
	else
	{
		loadPreset(preset.getId());
	}
}


void ClassSpaceChecker::loadPresetList(const QString &selectPresetId)
{
	initJarFileComboFlag_ = true;
	QList<CPresetData> presets = gSettingManager.getPresetList();

	ui.comboBox_JarFile->clear();

	int selected = -1;
	for(int i = 0; i < presets.size(); i++)
	{
		CPresetData preset = presets.at(i);
		if(preset.getId() == selectPresetId)
			selected = i;
		ui.comboBox_JarFile->addItem(preset.getKeyName(), preset.getId());
	}
	
	if(selected > -1) {
		ui.comboBox_JarFile->setCurrentIndex(selected);
	}

	ui.comboBox_JarFile->model()->sort(0);

	initJarFileComboFlag_ = false;
}


void ClassSpaceChecker::loadPreset(const QString &presetId)
{
	CPresetData preset = gSettingManager.getPresetDataWithId(presetId);

	QString mapPath = preset.getValue("mapPath").toString();

	ui.comboBox_JarFile->setEditText(preset.getKeyName());
	ui.lineEdit_MapFile->setText(mapPath);
}

void ClassSpaceChecker::saveCurrentPreset()
{
	QString jarPath = ui.comboBox_JarFile->currentText();

	CPresetData preset = gSettingManager.getPresetDataWithKeyName(jarPath);

	if(preset.isEmpty()) 
	{
		preset = gSettingManager.createPreset(jarPath);
	}

	QString mapPath = ui.lineEdit_MapFile->text();

	preset.setKeyName(jarPath);
	preset.setValue("mapPath", mapPath);

	gSettingManager.setLastPresetId(preset.getId());

	loadPresetList(preset.getId());
}


void ClassSpaceChecker::onCheckButtonClicked()
{
	QString jarPath = ui.comboBox_JarFile->currentText();
	QString mapPath = ui.lineEdit_MapFile->text();
	QString searchText = ui.lineEdit_Search->text();

	//jarPath = "C:\\work\\LINE_2013\\util\\TroublesomeRequester\\Download\\2013\\LINE.jar";
	//mapPath = "C:\\work\\LINE_2013\\util\\TroublesomeRequester\\Download\\2013\\pro_map.txt";

	removeAll();

	currentMapPath_ = mapPath;
	currentJarPath_ = jarPath;

	if(mapPath.isEmpty() == false)
	{
		ui.checkBox_ByUncryptName->setEnabled(true);
		ui.checkBox_ByUncryptName->setChecked(true);
		if(loadMapFile(mapPath) == false)
			return;
	}
	else
	{
		ui.checkBox_ByUncryptName->setEnabled(false);
		ui.checkBox_ByUncryptName->setChecked(false);
	}
	
	if(loadJarFile(jarPath))
	{
		collectData();

		searchClassByName(ui.checkBox_ByUncryptName->isChecked(), ui.checkBox_IgnoreInnerClass->isChecked(), searchText);
		analysisUniqueClassReport();
		analysisPackageReport();
	}

	ui.tabWidget->setCurrentIndex(0);

	saveCurrentPreset();
}


void ClassSpaceChecker::removeAll() 
{
	QList<ClassFileContext*>::iterator it = classList_.begin();
	for(; it != classList_.end(); it++)
	{
		ClassFileContext* ctx = *it;
		delete ctx;
	}

	QMap<QString, PackageContext*>::iterator it2 = packageMap_.begin();
	for(; it2 != packageMap_.end(); it2++)
	{
		PackageContext* ctx = it2.value();
		delete ctx;
	}

	QMap<QString, UniqueClassContext*>::iterator it3 = uniqueClassMap_.begin();
	for(; it3 != uniqueClassMap_.end(); it3++)
	{
		UniqueClassContext* ctx = it3.value();
		delete ctx;
	}

	uniqueClassMap_.clear();
	packageMap_.clear();
	classList_.clear();
	proguardMap_VK_.clear();

	ui.tableWidgetResult->clearContents();
	ui.tableWidgetResult->setRowCount(0);
	ui.tableWidgetPackageReport->clearContents();
	ui.tableWidgetPackageReport->setRowCount(0);
	ui.tableWidgetInnerClassReport->clearContents();
	ui.tableWidgetInnerClassReport->setRowCount(0);
}

bool ClassSpaceChecker::collectJavaClassInfo(const QString & classFile, ClassFileContext *ctx) 
{
	JavaClass *clazz = jclass_class_new(toMBCS(classFile).c_str(), NULL);

	if(clazz == NULL)
		return false;

	// method count
	ctx->methodCount = clazz->methods_count;
	ConstantPool *constant_pool = clazz->constant_pool;
		
	if(constant_pool == NULL)
		return false;

	char* class_name;
	char* method_name;
	char* package_name;
	char* this_class = jclass_cp_get_this_class_name(constant_pool);
	char* this_package = jclass_get_package_from_class_name(this_class);

	for(int count = 1; count < constant_pool->count; count++)
	{
		switch(constant_pool->entries[count].tag)
		{
		case CONSTANT_Class:
			class_name = jclass_cp_get_class_name(constant_pool, count, 1);
			package_name = jclass_get_package_from_class_name(class_name);

			if(!jclass_string_is_primitive_type(class_name))
			{
				if(strcmp(class_name, this_class) != 0)
				{
					//if(strchr(class_name,'$') == NULL)
					{
						ctx->classReferencedList.insert(class_name);
					}
				}
			}
			free(class_name);

			if(package_name != NULL)
			{	
				//if(do_flag & PACKAGES)
				//	string_list_add(package_list, package_name, IS_DEPENDENCY);
				free(package_name);
			}
			break;

		case CONSTANT_Methodref:
		case CONSTANT_InterfaceMethodref:
			//if(do_flag & METHOD_REF)
			//{
			//	method_name = jclass_cp_get_method_signature(constant_pool, count, 1);
			//	if(method_name != NULL)
			//	{
			//		class_name = jclass_get_class_from_method_signature(method_name);

			//		method_flag = (strcmp(class_name, this_class)? IS_DEPENDENCY: flag);
			//		
			//		if (constant_pool->entries[count].tag == CONSTANT_Methodref)
			//			string_list_add(method_list, method_name, method_flag);
			//		else
			//			string_list_add(interface_method_list, method_name, method_flag);

			//		free(class_name);
			//		free(method_name);
			//	}

			//}
			break;
		}
	}
	
	free(this_class);
	free(this_package);
	
	jclass_class_free(clazz);

	return true;
}


bool ClassSpaceChecker::loadJarFile(const QString & jarPath)
{
#if defined(Q_WS_WIN)
	QString f = jarPath;
	
	HZIP hz = OpenZip( (void *)f.toStdWString().c_str(), 0, ZIP_FILENAME );
	if( !hz ) 
	{
		QMessageBox::warning(this, "", tr("Jar file not found."));
		ui.comboBox_JarFile->setFocus();
		return false;
	}

	do{
		ZIPENTRYW ze; 
		ZRESULT zr = GetZipItem( hz, -1, &ze ); 
		if( zr != ZR_OK )
		{
			break;
		}
		int numitems = ze.index;

		installStatusProgressBar(numitems);

		for( int i = 0; i < numitems; i++ )
		{ 
			GetZipItem( hz, i, &ze );

			QString fileName = QString::fromStdWString(ze.name);
			if(fileName.indexOf(".class", 0, Qt::CaseInsensitive) < 0)
				continue;

			ClassFileContext *ctx = new ClassFileContext();
			
			ctx->filePath = ctx->className = fileName;
			ctx->fileSize = ze.unc_size;

			ctx->className.replace("/", ".");
			ctx->className.remove(".class");

			ctx->originalName = ctx->className;
			
			ctx->fullClassNameForKey = fileName;
			ctx->fullClassNameForKey.remove(".class");
			ctx->fullClassNameForKey.replace("/", "_");

			QString output = generateFileTempPath() + ctx->fullClassNameForKey + ".class";
			zr = UnzipItem(hz, i, (void*)output.toStdWString().c_str(), 0, ZIP_FILENAME);
			if( zr == ZR_OK ) 
			{
				ctx->decompiledBuffer = decompileClassAndReadFile(output);
		
				collectJavaClassInfo(output, ctx);
			}
			else 
			{
				ctx->referencedCount = -1;
				ctx->methodCount = -1;

				//if(fileName.indexOf("aux.class") < 0)
				//{
				//	QMessageBox::warning(this, "", tr("Jar file is invalid. Cannot unzip file : ") + fileName);
				//	removeAll();
				//	return false;
				//}
			}

			QFile f(output);
			f.remove();

			classList_.append(ctx);

			setStatusProgressValue(i + 1);
		}

		uninstallStatusProgressBar();
	} while( false );

	CloseZip(hz);

#else
	// TODO : other platform(MacOS) unzip patch file
	return false;
#endif
	return true;
}


bool ClassSpaceChecker::loadMapFile(const QString & mapPath) 
{
	QFile file(mapPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::warning(this, "", tr("Proguard Map file not found."));
		ui.lineEdit_MapFile->setFocus();
		return false;
	}

	while (!file.atEnd()) 
	{
		QByteArray line = file.readLine();
		QString lineStr(line);
		lineStr = lineStr.trimmed();

		if(lineStr.lastIndexOf(':') != lineStr.length() - 1) 
			continue;

		int pos = lineStr.indexOf("->");
		if(pos < 0)
			continue;

		QString key;
		QString value;

		key = lineStr.left(pos).trimmed();
		value = lineStr.mid(pos + 2).trimmed().remove(":");

		proguardMap_VK_.insert(value, key);
	}

	return true;
}


void ClassSpaceChecker::collectData()
{
	QList<ClassFileContext*>::iterator it = classList_.begin();

	installStatusProgressBar(classList_.size());

	for(int i = 0; it != classList_.end(); it++, i++)
	{
		ClassFileContext* ctx = *it;
		
		QMap<QString, QString>::iterator it = proguardMap_VK_.find(ctx->className);
		if(it != proguardMap_VK_.end())
		{
			ctx->originalName = it.value();
		}

		QList<ClassFileContext*>::iterator it2 = classList_.begin();
		for(int j = 0; it2 != classList_.end(); it2++, j++)
		{
			ClassFileContext* ctx2 = *it2;
			if(ctx2->classReferencedList.find(ctx->className) != ctx2->classReferencedList.end()) 
			{
				ctx->referencedCount++;
			}
		}
		//if(ctx->referencedCount == 0) 
		//{
		//	QList<ClassFileContext*>::iterator it2 = classList_.begin();
		//	for(int j = 0; it2 != classList_.end(); it2++, j++)
		//	{
		//		ClassFileContext* ctx2 = *it2;
		//		if(ctx2->decompiledBuffer.contains(ctx->className.toStdString().c_str())) 
		//		{
		//			ctx->referencedCount++;
		//		}
		//	}
		//}

		QString packageName;
		int pos = ctx->originalName.lastIndexOf(".");
		if(pos >= 0)
			packageName = ctx->originalName.left(pos);
		else
			packageName = ctx->originalName;

		QString uniqueClassName;
		{
			int pos = ctx->originalName.indexOf("$");
			if(pos >= 0)
				uniqueClassName = ctx->originalName.left(pos);
			else
				uniqueClassName = ctx->originalName;
		}
	
		PackageContext* ctxPackage = NULL;
		QMap<QString, PackageContext*>::iterator itPackage = packageMap_.find(packageName);
		if(itPackage == packageMap_.end())
		{
			ctxPackage = new PackageContext();
			ctxPackage->classCount = 0;
			ctxPackage->fileSize = 0;
			packageMap_.insert(packageName, ctxPackage);
		}
		else
		{
			ctxPackage = itPackage.value();
		}

		if(ctxPackage != NULL)
		{
			ctxPackage->packageName = packageName;
			ctxPackage->fileSize += ctx->fileSize;
			ctxPackage->classCount++;
			ctxPackage->uniqueClassNameSet.insert(uniqueClassName);
		}

		UniqueClassContext* ctxUniqueClass = NULL;
		QMap<QString, UniqueClassContext*>::iterator itUniqueClass = uniqueClassMap_.find(uniqueClassName);
		if(itUniqueClass == uniqueClassMap_.end())
		{
			ctxUniqueClass = new UniqueClassContext();
			ctxUniqueClass->classCount = 0;
			ctxUniqueClass->fileSize = 0;
			uniqueClassMap_.insert(uniqueClassName, ctxUniqueClass);
		}
		else
		{
			ctxUniqueClass = itUniqueClass.value();
		}

		if(ctxUniqueClass != NULL)
		{
			ctxUniqueClass->uniqueClassName = uniqueClassName;
			ctxUniqueClass->fileSize += ctx->fileSize;
			ctxUniqueClass->classCount++;
		}

		setStatusProgressValue(i + 1);
	}

	uninstallStatusProgressBar();
}

QByteArray ClassSpaceChecker::decompileClassAndReadFile(const QString &classFilePath) 
{	
	QString uri = qApp->applicationDirPath() + "\\jad\\jad.exe";

	// TODO
	//runProgram(uri, "-o -d \"" + generateFileTempPath() + "\" \"" + classFilePath + "\"", true, true);

	QString outputPath = classFilePath;
	//outputPath = outputPath.replace(".class", ".jad");

	QFile f(outputPath);
	if(!f.open(QIODevice::ReadOnly))
		return "";
	return f.readAll();
}

void ClassSpaceChecker::searchClassByName(bool useUncryptName, bool ignoreInnerClass, const QString & searchText) 
{
	searchClassInternal(searchText, true, useUncryptName, ignoreInnerClass);
}

void ClassSpaceChecker::searchClassByText(const QString & searchText) 
{
	searchClassInternal(searchText, false, false, false);
}


void ClassSpaceChecker::searchClassInternal(const QString & searchText, bool classNameMode, bool useUncryptName, bool ignoreInnerClass) 
{
	if(classList_.size() <= 0)
		return;

	ui.tableWidgetResult->clearContents();
	ui.tableWidgetResult->setRowCount(0);
	ui.tableWidgetResult->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);

	long totalSize = 0;
	int rowCount = 0;
	int methodCount = 0;
	QList<ClassFileContext*>::iterator it = classList_.begin();
	for(; it != classList_.end(); it++)
	{
		int col = 0;
		const ClassFileContext* ctx = *it;

		if(ignoreInnerClass && ctx->originalName.indexOf("$") > 0)
			continue;


		if(searchText.isEmpty() == false)
		{
			if(classNameMode)
			{
				if(useUncryptName)
				{
					if(ctx->originalName.contains(QRegExp(searchText, Qt::CaseInsensitive)) == false)
						continue;
				}
				else
				{
					if(ctx->className.contains(QRegExp(searchText, Qt::CaseInsensitive)) == false)
						continue;
				}
			}
			else
			{
				if(ctx->decompiledBuffer.contains(searchText.toStdString().c_str()) == false)
				//if(ctx->decompiledBuffer.contains(QRegExp(searchText, Qt::CaseInsensitive)) == false)
					continue;
			}
		}

		QString space = QString::number(ctx->fileSize);

		ui.tableWidgetResult->insertRow(rowCount);

		QTableWidgetItem *itemOriginal = new QTableWidgetItem(ctx->className);
		itemOriginal->setFlags(itemOriginal->flags() & ~Qt::ItemIsEditable);
		itemOriginal->setData(Qt::UserRole, qVariantFromValue((void *)ctx));
		ui.tableWidgetResult->setItem(rowCount, col++, itemOriginal);

		QTableWidgetItem *itemSize = new QTableWidgetItem();
		itemSize->setData(Qt::DisplayRole, ctx->fileSize);
		itemSize->setFlags(itemSize->flags() & ~Qt::ItemIsEditable);
		ui.tableWidgetResult->setItem(rowCount, col++, itemSize);

		QTableWidgetItem *itemCrypt = new QTableWidgetItem(ctx->originalName);
		itemCrypt->setFlags(itemCrypt->flags() & ~Qt::ItemIsEditable);
		ui.tableWidgetResult->setItem(rowCount, col++, itemCrypt);
		
		QTableWidgetItem *itemMethodCount = new QTableWidgetItem();
		itemMethodCount->setData(Qt::DisplayRole, ctx->methodCount);
		itemMethodCount->setFlags(itemMethodCount->flags() & ~Qt::ItemIsEditable);
		ui.tableWidgetResult->setItem(rowCount, col++, itemMethodCount);

		QTableWidgetItem *itemRefCount = new QTableWidgetItem();
		itemRefCount->setData(Qt::DisplayRole, ctx->referencedCount);
		itemRefCount->setFlags(itemRefCount->flags() & ~Qt::ItemIsEditable);
		ui.tableWidgetResult->setItem(rowCount, col++, itemRefCount);

		rowCount++;

		methodCount += ctx->methodCount;
		totalSize += ctx->fileSize;
	}

	QString resultStr;
	resultStr += "Total : ";
	resultStr += QString::number(rowCount);
	resultStr += " class found, ";
	resultStr += numberDot(QString::number(totalSize));
	resultStr += " bytes, ";
	resultStr += QString::number(methodCount);
	resultStr += " methods found";

	ui.lineEdit_Result->setText(resultStr);

	ui.tableWidgetResult->sortItems(0, Qt::AscendingOrder);
	ui.tableWidgetResult->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

	prevTotalResultStr_ = resultStr;
}



void ClassSpaceChecker::analysisUniqueClassReport()
{
	if(uniqueClassMap_.size() <= 0)
		return;

	ui.tableWidgetInnerClassReport->clearContents();
	ui.tableWidgetInnerClassReport->setRowCount(0);
	ui.tableWidgetInnerClassReport->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);

	int rowCount = 0;
	QMap<QString, UniqueClassContext*>::iterator it = uniqueClassMap_.begin();
	for(; it != uniqueClassMap_.end(); it++)
	{
		const UniqueClassContext* ctx = it.value();
		if(ctx->classCount < 2)
			continue;
		QString space = QString::number(ctx->fileSize);

		QTableWidgetItem *itemName = new QTableWidgetItem(ctx->uniqueClassName);
		itemName->setFlags(itemName->flags() & ~Qt::ItemIsEditable);
		itemName->setData(Qt::UserRole, qVariantFromValue((void *)ctx));

		QTableWidgetItem *itemClassCount = new QTableWidgetItem();
		itemClassCount->setData(Qt::DisplayRole, ctx->classCount - 1);
		itemClassCount->setFlags(itemClassCount->flags() & ~Qt::ItemIsEditable);

		QTableWidgetItem *itemSize = new QTableWidgetItem();
		itemSize->setData(Qt::DisplayRole, ctx->fileSize);
		itemSize->setFlags(itemSize->flags() & ~Qt::ItemIsEditable);

		ui.tableWidgetInnerClassReport->insertRow(rowCount);
		ui.tableWidgetInnerClassReport->setItem(rowCount, 0, itemName);
		ui.tableWidgetInnerClassReport->setItem(rowCount, 1, itemClassCount);
		ui.tableWidgetInnerClassReport->setItem(rowCount, 2, itemSize);

		rowCount++;
	}

	ui.tableWidgetInnerClassReport->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}


void ClassSpaceChecker::analysisPackageReport() 
{
	if(packageMap_.size() <= 0)
		return;

	ui.tableWidgetPackageReport->clearContents();
	ui.tableWidgetPackageReport->setRowCount(0);
	ui.tableWidgetPackageReport->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);

	int rowCount = 0;
	QMap<QString, PackageContext*>::iterator it = packageMap_.begin();
	for(; it != packageMap_.end(); it++)
	{
		const PackageContext* ctx = it.value();
		QString space = QString::number(ctx->fileSize);

		QTableWidgetItem *itemOriginal = new QTableWidgetItem(ctx->packageName);
		itemOriginal->setFlags(itemOriginal->flags() & ~Qt::ItemIsEditable);
		itemOriginal->setData(Qt::UserRole, qVariantFromValue((void *)ctx));

		QTableWidgetItem *itemClassCount = new QTableWidgetItem();
		itemClassCount->setData(Qt::DisplayRole, ctx->classCount);
		itemClassCount->setFlags(itemClassCount->flags() & ~Qt::ItemIsEditable);

		QTableWidgetItem *itemUniqueClassCount = new QTableWidgetItem();
		itemUniqueClassCount->setData(Qt::DisplayRole, ctx->uniqueClassNameSet.size());
		itemUniqueClassCount->setFlags(itemUniqueClassCount->flags() & ~Qt::ItemIsEditable);

		QTableWidgetItem *itemDiffCount = new QTableWidgetItem();
		itemDiffCount->setData(Qt::DisplayRole, (ctx->classCount - ctx->uniqueClassNameSet.size()));
		itemDiffCount->setFlags(itemDiffCount->flags() & ~Qt::ItemIsEditable);

		QTableWidgetItem *itemSize = new QTableWidgetItem();
		itemSize->setData(Qt::DisplayRole, ctx->fileSize);
		itemSize->setFlags(itemSize->flags() & ~Qt::ItemIsEditable);

		ui.tableWidgetPackageReport->insertRow(rowCount);
		ui.tableWidgetPackageReport->setItem(rowCount, 0, itemOriginal);
		ui.tableWidgetPackageReport->setItem(rowCount, 1, itemClassCount);
		ui.tableWidgetPackageReport->setItem(rowCount, 2, itemUniqueClassCount);
		ui.tableWidgetPackageReport->setItem(rowCount, 3, itemDiffCount);
		ui.tableWidgetPackageReport->setItem(rowCount, 4, itemSize);

		rowCount++;
	}

	ui.tableWidgetPackageReport->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}


void ClassSpaceChecker::writeToCSVFile(const QTableWidget *tableWidget, const QString & outputPath)
{
	QFile outputFile(outputPath);
	if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
	{
		QMessageBox::warning(this, "", tr("Failed to create csv file."));
		ui.lineEdit_MapFile->setFocus();
		return;
	}

	for(int i = 0; i < tableWidget->columnCount(); i++) 
	{
		if(i != 0)
		{
			outputFile.write(",");
		}

		QTableWidgetItem *header = tableWidget->horizontalHeaderItem(i);
		outputFile.write("\"");
		outputFile.write(header->text().toStdString().c_str());
		outputFile.write("\"");
	}
	outputFile.write("\n");

	for(int i = 0; i < tableWidget->rowCount(); i++)
	{
		for(int j = 0; j < tableWidget->columnCount(); j++)
		{
			if(j != 0)
			{
				outputFile.write(",");
			}

			QTableWidgetItem *item = tableWidget->item(i, j);

			outputFile.write("\"");
			outputFile.write(item->text().toStdString().c_str());
			outputFile.write("\"");
		}
		outputFile.write("\n");
	}

	outputFile.flush();
	outputFile.close();
}

void ClassSpaceChecker::onChangedSearchClassName(QString text)
{
	ui.tabWidget->setCurrentIndex(0);
	searchClassByName(ui.checkBox_ByUncryptName->isChecked(), ui.checkBox_IgnoreInnerClass->isChecked(), text);
}

void ClassSpaceChecker::onChangedSearchText(QString text)
{
	ui.tabWidget->setCurrentIndex(0);
	searchClassByText(text);
}


void ClassSpaceChecker::onClickedJarFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Jar File"), ui.comboBox_JarFile->currentText(), tr("Jar Files (*.jar *.zip)"));
 
	if(fileName.isEmpty() == false)
	{
		checkAndJarFilePreset(fileName);
		ui.comboBox_JarFile->setFocus();
	}
}


void ClassSpaceChecker::onClickedMapFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Proguard Map File"), ui.lineEdit_MapFile->text(), tr("Map Files (*.txt)"));
 
	if(fileName.isEmpty() == false)
	{
		ui.lineEdit_MapFile->setText(fileName);
		ui.lineEdit_MapFile->setFocus();
	}
}

void ClassSpaceChecker::onClickedExportCSV()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Export to CSV File"), tr(""), tr("CSV Files (*.csv)"));
	if(fileName.isEmpty())
		return;

	QTableWidget *table = NULL;
	int idx = ui.tabWidget->currentIndex();
	if(idx == 0)
		table = ui.tableWidgetResult;
	else if(idx == 1)
		table = ui.tableWidgetPackageReport;
	else
		table = ui.tableWidgetInnerClassReport;

	writeToCSVFile(table, fileName);
}

int ClassSpaceChecker::getIntFromTableItem(QTableWidget *table, int row, int column, int def) 
{
	QTableWidgetItem *itemSize = table->item(row, column);
	if(itemSize == NULL)
		return def;
	return itemSize->data(Qt::DisplayRole).toInt();
}

void ClassSpaceChecker::onResultItemSelectionChanged()
{
	QList<QTableWidgetItem *> items = ui.tableWidgetResult->selectedItems();
	if(items.size() <= 0)
	{
		ui.lineEdit_Result->setText(prevTotalResultStr_);
		return;
	}

	QSet<int> set;
	int classCount = 0;
	int methodCount = 0;
	int totalSize = 0;
	for(int i = 0; i < items.size(); i++) 
	{
		QTableWidgetItem *item = items.at(i);
		int row = item->row();
		if(set.find(row) != set.end())
			continue;

		totalSize += getIntFromTableItem(ui.tableWidgetResult, item->row(), 1);
		methodCount += getIntFromTableItem(ui.tableWidgetResult, item->row(), 3);

		classCount++;

		set.insert(row);
	}


	QString resultStr;
	resultStr += "Selected Count : ";
	resultStr += QString::number(classCount);
	resultStr += ", FileSize : ";
	resultStr += numberDot(QString::number(totalSize));
	resultStr += " bytes";
	resultStr += ", Method : ";
	resultStr += QString::number(methodCount);

	ui.lineEdit_Result->setText(resultStr);
}

void ClassSpaceChecker::onPackageReportItemSelectionChanged()
{
	QList<QTableWidgetItem *> items = ui.tableWidgetPackageReport->selectedItems();
	if(items.size() <= 0)
	{
		ui.lineEdit_Result->setText(prevTotalResultStr_);
		return;
	}

	QSet<int> set;
	int classCount = 0;
	int uniqueClassCount = 0;
	int totalSize = 0;
	for(int i = 0; i < items.size(); i++) 
	{
		QTableWidgetItem *item = items.at(i);
		int row = item->row();
		if(set.find(row) != set.end())
			continue;

		QTableWidgetItem *itemClassCount = ui.tableWidgetPackageReport->item(item->row(), 1);
		if(itemClassCount == NULL)
			continue;
		classCount += itemClassCount->data(Qt::DisplayRole).toInt();

		QTableWidgetItem *itemUniqueClassCount = ui.tableWidgetPackageReport->item(item->row(), 2);
		if(itemUniqueClassCount == NULL)
			continue;
		uniqueClassCount += itemUniqueClassCount->data(Qt::DisplayRole).toInt();

		QTableWidgetItem *itemFileSize = ui.tableWidgetPackageReport->item(item->row(), 4);
		if(itemFileSize == NULL)
			continue;
		totalSize += itemFileSize->data(Qt::DisplayRole).toInt();
		set.insert(row);
	}


	QString resultStr;
	resultStr += QString::number(classCount);
	resultStr += " class count, ";
	resultStr += QString::number(uniqueClassCount);
	resultStr += " unique class count, ";
	resultStr += numberDot(QString::number(totalSize));
	resultStr += " bytes";

	ui.lineEdit_Result->setText(resultStr);
}

void ClassSpaceChecker::onInnerClassReportItemSelectionChanged()
{
	QList<QTableWidgetItem *> items = ui.tableWidgetInnerClassReport->selectedItems();
	if(items.size() <= 0)
	{
		ui.lineEdit_Result->setText(prevTotalResultStr_);
		return;
	}

	QSet<int> set;
	int classCount = 0;
	int uniqueClassCount = 0;
	int totalSize = 0;
	for(int i = 0; i < items.size(); i++) 
	{
		QTableWidgetItem *item = items.at(i);
		int row = item->row();
		if(set.find(row) != set.end())
			continue;

		QTableWidgetItem *itemClassCount = ui.tableWidgetInnerClassReport->item(item->row(), 1);
		if(itemClassCount == NULL)
			continue;
		classCount += itemClassCount->data(Qt::DisplayRole).toInt();

		QTableWidgetItem *itemFileSize = ui.tableWidgetInnerClassReport->item(item->row(), 2);
		if(itemFileSize == NULL)
			continue;
		totalSize += itemFileSize->data(Qt::DisplayRole).toInt();
		set.insert(row);
	}


	QString resultStr;
	resultStr += QString::number(set.size());
	resultStr += " selected, ";
	resultStr += QString::number(classCount);
	resultStr += " inner class count, ";
	resultStr += numberDot(QString::number(totalSize));
	resultStr += " bytes";

	ui.lineEdit_Result->setText(resultStr);
}

void ClassSpaceChecker::onPackageReportCellDoubleClicked(int row, int column)
{
	
}

void ClassSpaceChecker::onResultCellDoubleClicked(int row, int column)
{
	QTableWidgetItem *item = ui.tableWidgetResult->item(row, 0);
	if(item == NULL)
		return;

	const ClassFileContext *ctx = reinterpret_cast<ClassFileContext *>(item->data(Qt::UserRole).value<void *>());
	if(ctx == NULL)
		return;

	openClassFile(currentJarPath_, ctx);
}

void ClassSpaceChecker::onTabCurrentChanged(int index)
{
	ui.lineEdit_Result->setText(prevTotalResultStr_);
}


unsigned long ClassSpaceChecker::runProgram(const QString &theUri, const QString &param, bool silentMode, bool waitExit)
{
	QString uri = QDir::toNativeSeparators(theUri);

	std::wstring uriW = uri.toStdWString().c_str();
	std::wstring paramW = param.toStdWString().c_str();

	SHELLEXECUTEINFOW shellExInfo;
	memset(&shellExInfo, 0, sizeof(SHELLEXECUTEINFO));
	shellExInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shellExInfo.hwnd = NULL;
	shellExInfo.lpVerb = L"open";
	shellExInfo.lpFile = uriW.c_str();
	shellExInfo.lpParameters = paramW.c_str();
	shellExInfo.lpDirectory = NULL;
	shellExInfo.nShow = silentMode ? SW_HIDE : SW_SHOWNORMAL;
	shellExInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
	shellExInfo.hInstApp = NULL;

	ShellExecuteEx(&shellExInfo);

	if(waitExit)
		WaitForSingleObject(shellExInfo.hProcess,INFINITE);

	unsigned long processId = GetProcessId(shellExInfo.hProcess); 


	return processId;
}

void ClassSpaceChecker::openClassFile(const QString jarPath, const ClassFileContext *ctx)
{
	bool ok = false;

#if defined(Q_WS_WIN)
	QString f = jarPath;
	
	HZIP hz = OpenZip( (void *)f.toStdWString().c_str(), 0, ZIP_FILENAME );
	if( !hz ) 
	{
		QMessageBox::warning(this, "", tr("Jar file not found."));
		ui.comboBox_JarFile->setFocus();
		return;
	}

	do{
		ZIPENTRYW ze; 
		ZRESULT zr = GetZipItem( hz, -1, &ze ); 
		if( zr != ZR_OK )
			break;
		
		int i; 
		zr = FindZipItem(hz, ctx->filePath.toStdWString().c_str(), true, &i, &ze);
		if( zr != ZR_OK )
			break;

		//QString output = generateFileTempPath() + "Temp.class";
		QString output = generateFileTempPath() + ctx->fullClassNameForKey + ".class";
		zr = UnzipItem(hz, i, (void*)output.toStdWString().c_str(), 0, ZIP_FILENAME);
		if( zr != ZR_OK )
			break;

		QString uri = qApp->applicationDirPath() + "\\jd-gui\\jd-gui.exe";
		uri = QDir::toNativeSeparators(uri);

		if(prevJdProcessId_ != 0)
		{
			HANDLE h = OpenProcess(PROCESS_TERMINATE, false, prevJdProcessId_);
			if(h != NULL)
			{
				TerminateProcess(h, 1);
				CloseHandle(h);
			}
			prevJdProcessId_ = 0;
		}

		prevJdProcessId_ = runProgram(uri, "\"" + output + "\"", false);

		ok = true;
	} while( false );

	CloseZip(hz);

#else
	// TODO : other platform(MacOS) unzip patch file
	return false;
#endif

	if(ok == false)
	{
		QMessageBox::warning(this, "", tr("Can not open this class file."));
	}
}


void ClassSpaceChecker::onJarFileCurrentIndexChanged(int index)
{
	if(initJarFileComboFlag_)
		return;

	QString presetId = ui.comboBox_JarFile->itemData(index).toString();
	loadPreset(presetId);
}


void ClassSpaceChecker::onJarFileEditTextChanged(QString text)
{
	if(initJarFileComboFlag_)
		return;

	checkAndJarFilePreset(text);
}

void ClassSpaceChecker::onClickedDelete()
{
	if(ui.comboBox_JarFile->count() < 0 ||  ui.comboBox_JarFile->currentIndex() < 0)
		return;

	if(QMessageBox::question(this, "", "Are you sure you remove this preset?", QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes)
		return;

	QString presetId = ui.comboBox_JarFile->itemData( ui.comboBox_JarFile->currentIndex() ).toString();

	gSettingManager.removeValue(presetId);

	ui.comboBox_JarFile->removeItem(ui.comboBox_JarFile->currentIndex());

	loadPresetList("");
}


void ClassSpaceChecker::onClickedIgnoreInnerClass()
{
	ui.tabWidget->setCurrentIndex(0);
	searchClassByName(ui.checkBox_ByUncryptName->isChecked(), ui.checkBox_IgnoreInnerClass->isChecked(), ui.lineEdit_Search->text());
}


void ClassSpaceChecker::onClickedByUncryptName()
{
	ui.tabWidget->setCurrentIndex(0);
	searchClassByName(ui.checkBox_ByUncryptName->isChecked(), ui.checkBox_IgnoreInnerClass->isChecked(), ui.lineEdit_Search->text());
}
