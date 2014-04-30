#ifndef SOURCEVIEWER_H
#define SOURCEVIEWER_H

#include <QtGui>
#include "ui_sourceviewer.h"

class SourceViewContext
{
public:
	QTextEdit *logTextEdit;
};

typedef QMap<QString, SourceViewContext* > QTextEditorContextMap;

class SourceViewer : public QDialog
{
	Q_OBJECT

public:
	SourceViewer(QWidget *parent = 0);
	~SourceViewer();

	void setSourceText(const QString &key, const QString &name, const QString &searchText, const QByteArray &sourceTextBuffer);
	void doHighlightText(const QString &text, bool caseSensitive);
	QTextEdit* getCurrentSourceEdit();

protected slots:
	void onSelectionChanged();
	void onChangedSearchText(QString text);
	void onClickedSearchPrev();
	void onClickedSearchNext();
	void onClickedCaseSensitive(bool checked);

protected:
	bool eventFilter(QObject *object, QEvent *evt);
	void closeEvent( QCloseEvent *evt );

private:
	Ui::SourceViewer ui;
	QTextEditorContextMap logTextWidgetMap_;
	bool workingHighlightText_;
};

#endif // SOURCEVIEWER_H
