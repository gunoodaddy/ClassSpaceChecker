#include "classspacechecker.h"
#include <QtGui>
#include <qt_windows.h>
#include "XZip/XUnZip.h"

ClassSpaceChecker::ClassSpaceChecker(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), prevJdProcessId_(0)
{
	ui.setupUi(this);
	ui.tableWidgetResult->horizontalHeader()->setResizeMode( QHeaderView::Interactive );

	ui.lineEdit_JarFile->setDragEnabled(true);
	ui.lineEdit_JarFile->installEventFilter( this );
	ui.lineEdit_MapFile->setDragEnabled(true);
	ui.lineEdit_MapFile->installEventFilter( this );
	ui.tableWidgetResult->installEventFilter( this );

	ui.lineEdit_JarFile->setText(getSetting("jarPath").toString());
	ui.lineEdit_MapFile->setText(getSetting("mapPath").toString());
	ui.lineEdit_Search->setText(getSetting("searchText").toString());

	ui.lineEdit_Result->setText("Press Analysis button first.");

	updateWindowTitle();
}

ClassSpaceChecker::~ClassSpaceChecker()
{

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

	if(object == ui.lineEdit_JarFile 
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
						if(object == ui.lineEdit_JarFile)
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
						ui.lineEdit_JarFile->setText(path);
				}
			}
		}
	}
	return QMainWindow::eventFilter(object, evt);
}

void ClassSpaceChecker::onCheckButtonClicked()
{
	QString jarPath = ui.lineEdit_JarFile->text();
	QString mapPath = ui.lineEdit_MapFile->text();
	QString searchText = ui.lineEdit_Search->text();

	//jarPath = "C:\\work\\LINE_2013\\util\\TroublesomeRequester\\Download\\2013\\LINE.jar";
	//mapPath = "C:\\work\\LINE_2013\\util\\TroublesomeRequester\\Download\\2013\\pro_map.txt";

	setSetting("jarPath", jarPath);
	setSetting("mapPath", mapPath);
	setSetting("searchText", searchText);

	removeAll();

	currentMapPath_ = mapPath;
	currentJarPath_ = jarPath;

	changeResultHeader();

	if(mapPath.isEmpty() == false)
	{
		if(loadMapFile(mapPath) == false)
			return;
	}
	
	if(loadJarFile(jarPath))
	{
		collectClassFile();
		searchClass(ui.checkBox_ByUncryptName->isChecked(), searchText);
	}
}

void ClassSpaceChecker::changeResultHeader()
{
	if(currentMapPath_.isEmpty() == false)
	{
		ui.checkBox_ByUncryptName->setChecked(true);
		ui.checkBox_ByUncryptName->setEnabled(true);
		ui.tableWidgetResult->setColumnCount(3);
		ui.tableWidgetResult->setHorizontalHeaderLabels(QString("Class Name;File Size;Uncrypted Name").split(";"));  
	}
	else
	{
		ui.checkBox_ByUncryptName->setChecked(false);
		ui.checkBox_ByUncryptName->setEnabled(false);
		ui.tableWidgetResult->setColumnCount(2);
		ui.tableWidgetResult->setHorizontalHeaderLabels(QString("Class Name;File Size;").split(";"));  
	}
}

void ClassSpaceChecker::removeAll() 
{
	QList<ClassFileContext*>::iterator it = classList_.begin();
	for(; it != classList_.end(); it++)
	{
		ClassFileContext* ctx = *it;
		delete ctx;
	}
	classList_.clear();
	proguardMap_VK_.clear();
}

bool ClassSpaceChecker::loadJarFile(const QString & jarPath)
{
#if defined(Q_WS_WIN)
	QString f = jarPath;
	
	HZIP hz = OpenZip( (void *)f.toStdWString().c_str(), 0, ZIP_FILENAME );
	if( !hz ) 
	{
		QMessageBox::warning(this, "", tr("Jar file not found."));
		ui.lineEdit_JarFile->setFocus();
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

			classList_.append(ctx);
		}
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


void ClassSpaceChecker::collectClassFile()
{
	if(proguardMap_VK_.size() <= 0)
		return;
		
	QList<ClassFileContext*>::iterator it = classList_.begin();
	for(; it != classList_.end(); it++)
	{
		ClassFileContext* ctx = *it;
		
		QMap<QString, QString>::iterator it = proguardMap_VK_.find(ctx->className);
		if(it != proguardMap_VK_.end())
		{
			ctx->originalName = it.value();
		}
	}
}

void ClassSpaceChecker::searchClass(bool useUncryptName, const QString & searchText) 
{
	if(classList_.size() <= 0)
		return;

	ui.tableWidgetResult->clearContents();
	ui.tableWidgetResult->setRowCount(0);
	ui.tableWidgetResult->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);

	long totalSize = 0;
	int rowCount = 0;
	QList<ClassFileContext*>::iterator it = classList_.begin();
	for(; it != classList_.end(); it++)
	{
		const ClassFileContext* ctx = *it;
		if(searchText.isEmpty() == false)
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

		QString space = QString::number(ctx->fileSize);

		QTableWidgetItem *itemOriginal = new QTableWidgetItem(ctx->className);
		itemOriginal->setFlags(itemOriginal->flags() & ~Qt::ItemIsEditable);
		itemOriginal->setData(Qt::UserRole, qVariantFromValue((void *)ctx));

		QTableWidgetItem *itemSize = new QTableWidgetItem();
		itemSize->setData(Qt::DisplayRole, ctx->fileSize);
		itemSize->setFlags(itemSize->flags() & ~Qt::ItemIsEditable);

		ui.tableWidgetResult->insertRow(rowCount);
		ui.tableWidgetResult->setItem(rowCount, 0, itemOriginal);
		ui.tableWidgetResult->setItem(rowCount, 1, itemSize);

		if(ui.tableWidgetResult->columnCount() > 2)
		{
			QTableWidgetItem *itemCrypt = new QTableWidgetItem(ctx->originalName);
			itemCrypt->setFlags(itemCrypt->flags() & ~Qt::ItemIsEditable);
			ui.tableWidgetResult->setItem(rowCount, 2, itemCrypt);
		}

		rowCount++;

		totalSize += ctx->fileSize;
	}

	QString resultStr;
	resultStr += "Total : ";
	resultStr += QString::number(rowCount);
	resultStr += " class found, ";
	resultStr += numberDot(QString::number(totalSize));
	resultStr += " bytes";

	ui.lineEdit_Result->setText(resultStr);

	ui.tableWidgetResult->sortItems(0, Qt::AscendingOrder);
	ui.tableWidgetResult->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

	prevTotalResultStr_ = resultStr;
}

void ClassSpaceChecker::onChangedSearchText(QString text)
{
	setSetting("searchText", text);

	searchClass(ui.checkBox_ByUncryptName->isChecked(), text);
}


void ClassSpaceChecker::onClickedJarFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Jar File"), "", tr("Jar Files (*.jar *.zip)"));
 
	if(fileName.isEmpty() == false)
	{
		ui.lineEdit_JarFile->setText(fileName);
		ui.lineEdit_JarFile->setFocus();
	}
}


void ClassSpaceChecker::onClickedMapFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Proguard Map File"), "", tr("Map Files (*.txt)"));
 
	if(fileName.isEmpty() == false)
	{
		ui.lineEdit_MapFile->setText(fileName);
		ui.lineEdit_MapFile->setFocus();
	}
}

void ClassSpaceChecker::onClickedByUncryptName()
{
	searchClass(ui.checkBox_ByUncryptName->isChecked(), ui.lineEdit_Search->text());
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
	int totalSize = 0;
	for(int i = 0; i < items.size(); i++) 
	{
		QTableWidgetItem *item = items.at(i);
		int row = item->row();
		if(set.find(row) != set.end())
			continue;

		QTableWidgetItem *itemSize = ui.tableWidgetResult->item(item->row(), 1);
		if(itemSize == NULL)
			continue;

		int data = itemSize->data(Qt::DisplayRole).toInt();
		totalSize += data;
		classCount++;
		set.insert(row);
	}


	QString resultStr;
	resultStr += "Total : ";
	resultStr += QString::number(classCount);
	resultStr += " class selected, ";
	resultStr += numberDot(QString::number(totalSize));
	resultStr += " bytes";

	ui.lineEdit_Result->setText(resultStr);
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




void ClassSpaceChecker::openClassFile(const QString jarPath, const ClassFileContext *ctx)
{
	bool ok = false;

#if defined(Q_WS_WIN)
	QString f = jarPath;
	
	HZIP hz = OpenZip( (void *)f.toStdWString().c_str(), 0, ZIP_FILENAME );
	if( !hz ) 
	{
		QMessageBox::warning(this, "", tr("Jar file not found."));
		ui.lineEdit_JarFile->setFocus();
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

		QString uri = qApp->applicationDirPath() + "\\jd\\jd-gui.exe";
		uri = QDir::toNativeSeparators(uri);

		std::wstring uriW = uri.toStdWString().c_str();
		std::wstring paramW = L"\"";
		paramW += output.toStdWString().c_str();
		paramW += L"\"";

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

		SHELLEXECUTEINFOW shellExInfo;
		memset(&shellExInfo, 0, sizeof(SHELLEXECUTEINFO));
		shellExInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		shellExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shellExInfo.hwnd = NULL;
		shellExInfo.lpVerb = L"open";
		shellExInfo.lpFile = uriW.c_str();
		shellExInfo.lpParameters = paramW.c_str();
		shellExInfo.lpDirectory = NULL;
		shellExInfo.nShow = SW_SHOWNORMAL;
		shellExInfo.hInstApp = NULL;

		ShellExecuteEx(&shellExInfo);

		prevJdProcessId_ = GetProcessId(shellExInfo.hProcess); 

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