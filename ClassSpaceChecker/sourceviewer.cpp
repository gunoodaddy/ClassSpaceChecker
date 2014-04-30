#include "stdafx.h"
#include "sourceviewer.h"
#include "GlobalEvent.h"

SourceViewer::SourceViewer(QWidget *parent)
	: QDialog(parent), workingHighlightText_(false)
{
	ui.setupUi(this);

	QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+W"), this);
	shortcut = new QShortcut(QKeySequence("F4"), this);
	QObject::connect(shortcut, SIGNAL(activated()), ui.prevBtn, SLOT(click()));

	shortcut = new QShortcut(QKeySequence("F3"), this);
	QObject::connect(shortcut, SIGNAL(activated()), ui.nextBtn, SLOT(click()));

	ui.caseSensitiveCheck->setChecked(true);
}

SourceViewer::~SourceViewer()
{
	QTextEditorContextMap::const_iterator i = logTextWidgetMap_.begin();
	for(; i != logTextWidgetMap_.end(); i++) 
	{
		SourceViewContext * logTextEditCtx = i.value();
		delete logTextEditCtx->logTextEdit;
		delete logTextEditCtx;
	}
	logTextWidgetMap_.clear();
}

bool SourceViewer::eventFilter(QObject *object, QEvent *evt)
{
	//if(evt->type() == QEvent::KeyPress) 
	//{
	//	QKeyEvent *e = (QKeyEvent*)evt;
	//	if(e->key() == Qt::Key_Enter)
	//	{
	//		if(ui.searchText->hasFocus())
	//		{
	//			onClickedSearchNext();
	//			return true;
	//		}
	//	}
	//}

	return QDialog::eventFilter(object, evt);
}

void SourceViewer::setSourceText(const QString &key, const QString &name, const QString &searchText, const QByteArray &sourceTextBuffer)
{
	QTextEdit *logTextEdit = NULL;
	QTextEditorContextMap::const_iterator i = logTextWidgetMap_.find(key);
	if(i == logTextWidgetMap_.end())
	{
		// New tab created..
		logTextEdit = new QTextEdit();
		QObject::connect(logTextEdit, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
		logTextEdit->setReadOnly(true);

		SourceViewContext* ctx = new SourceViewContext();
		ctx->logTextEdit = logTextEdit;
		logTextWidgetMap_.insert(key, ctx);
		ui.tabWidget->insertTab(0, logTextEdit, name);
		ui.tabWidget->setCurrentIndex(0);
	}
	else
	{
		// Already the tab exists.
		logTextEdit = i.value()->logTextEdit;
		int index = ui.tabWidget->indexOf(logTextEdit);
		if(index >= 0)
		{
			ui.tabWidget->setCurrentIndex(index);
		}
	}

	workingHighlightText_ = true;

	ui.searchText->setText(searchText);

	logTextEdit->setText(sourceTextBuffer);

	logTextEdit->setFocus();

	if(searchText.isEmpty() == false) 
	{
		doHighlightText(searchText, ui.caseSensitiveCheck->isChecked());
	
		QTextDocument::FindFlags flags = 0x0;
		if(ui.caseSensitiveCheck->isChecked())
			flags |=  QTextDocument::FindCaseSensitively;

		logTextEdit->moveCursor(QTextCursor::Start);

		while(logTextEdit->find(searchText, flags))
		{
			break;
		}
	}

	workingHighlightText_ = false;
}


QTextEdit* SourceViewer::getCurrentSourceEdit() 
{
	int index = ui.tabWidget->currentIndex();
	if(index >= 0)
	{
		QTextEdit *textEdit = (QTextEdit*)ui.tabWidget->widget(index);
		return textEdit;
	}
	return NULL;
}

void SourceViewer::doHighlightText(const QString &text, bool caseSensitive)
{
	qDebug() << "doHighlightText : text=" << text;

	workingHighlightText_ = true;
	do
	{
		QTextEdit *logTextEdit = getCurrentSourceEdit();
		if(logTextEdit == NULL)
			break;

		int orgValueVert = logTextEdit->verticalScrollBar()->value();
		int orgValueHor = logTextEdit->horizontalScrollBar()->value();
		QTextCursor orgCursor = logTextEdit->textCursor();
		QTextDocument::FindFlags flags = 0x0;
		if(caseSensitive)
			flags |=  QTextDocument::FindCaseSensitively;

		QList<QTextEdit::ExtraSelection> extraSelections;
		logTextEdit->moveCursor(QTextCursor::Start);

		QColor fgColor = QColor("white");
		QColor bgColor = QColor("#C32438");

		while(logTextEdit->find(text, flags))
		{
			QTextEdit::ExtraSelection extra;
			extra.format.setForeground(fgColor);
			extra.format.setBackground(bgColor);
			extra.cursor = logTextEdit->textCursor();
			extraSelections.append(extra);
		}

		logTextEdit->setExtraSelections(extraSelections);
		logTextEdit->setTextCursor(orgCursor);
		logTextEdit->verticalScrollBar()->setValue(orgValueVert);
		logTextEdit->horizontalScrollBar()->setValue(orgValueHor);
	} while(false);
	workingHighlightText_ = false;
}

void SourceViewer::onChangedSearchText(QString text)
{
	if(workingHighlightText_)
		return;

	doHighlightText(text, ui.caseSensitiveCheck->isChecked());
}

void SourceViewer::onSelectionChanged()
{
	if(workingHighlightText_)
		return;

	if(gGlobalEvent.isMousePressed() == true)
		return;

	QTextEdit *logTextEdit = getCurrentSourceEdit();
	if(logTextEdit != NULL)
	{
		QString s = logTextEdit->textCursor().selectedText();

		if(s.isEmpty() == false && s.length() < 50)
		{
			qDebug() << "onLogSelectionChanged : " << gGlobalEvent.isMousePressed() << s;

			ui.searchText->setText(s);
		}
	}
}

void SourceViewer::onClickedSearchPrev()
{
	QTextEdit *logTextEdit = getCurrentSourceEdit();
	if(logTextEdit != NULL)
	{
		QTextDocument::FindFlags flags = QTextDocument::FindBackward;
		if(ui.caseSensitiveCheck->isChecked())
			flags |=  QTextDocument::FindCaseSensitively;

		bool find = logTextEdit->find(ui.searchText->text(), flags);
		if(find == false)
		{
			logTextEdit->moveCursor(QTextCursor::End);
			logTextEdit->horizontalScrollBar()->setValue(0);
			logTextEdit->find(ui.searchText->text(), flags);
		}
	}
}

void SourceViewer::onClickedSearchNext()
{
	QTextEdit *logTextEdit = getCurrentSourceEdit();
	if(logTextEdit != NULL)
	{
		QTextDocument::FindFlags flags = 0x0;
		if(ui.caseSensitiveCheck->isChecked())
			flags |=  QTextDocument::FindCaseSensitively;

		bool find = logTextEdit->find(ui.searchText->text(), flags);
		if(find == false)
		{
			logTextEdit->moveCursor(QTextCursor::Start);
			logTextEdit->find(ui.searchText->text(), flags);
		}
	}
}

void SourceViewer::onClickedCaseSensitive(bool checked)
{
	doHighlightText(ui.searchText->text(), checked);
}


void SourceViewer::closeEvent( QCloseEvent *evt )
{


	QDialog::closeEvent( evt );
}