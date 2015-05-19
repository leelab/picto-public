#ifndef SCRIPTTEXTEDIT_H
#define SCRIPTTEXTEDIT_H
#include <QEvent>
#include "SearchablePlainTextEdit.h"
#include "ScriptSyntaxHighlighter.h"
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

/*! \brief A Text Editor widget for Picto scripts.
 *	\details This is the widget used in the Picto Property window in the Designer for editing Picto scripts.  It includes
 *	syntax highlighting based on the ScriptSyntaxHighlighter and is searchable such that the searched term is highlighted.
 *	\note This widget is used by the ScriptWidget which handles search commands and some other functionality.
 *	\sa ScriptWidget
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class ScriptTextEdit : public SearchablePlainTextEdit
{
	Q_OBJECT
public: 
	ScriptTextEdit(bool singleLine = false);

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth() const;

protected:
	virtual void applyExtraSelections();
	virtual void focusOutEvent(QFocusEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

	QList<QTextEdit::ExtraSelection> lineHighlight_;

private:
	ScriptSyntaxHighlighter *syntaxHighlighter_;
	QWidget *lineNumberArea;

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void highlightCurrentLine();
	void updateLineNumberArea(const QRect &, int);
};

/*! \brief A widget to display ScriptTextEdit line numbers 
 */
class LineNumberArea : public QWidget
{
public:
	//! Construct a new LineNumberArea
	LineNumberArea(ScriptTextEdit *editor) : QWidget(editor) {
		codeEditor = editor;
	}

	//! Override default sizeHint
	QSize sizeHint() const {
		return QSize(codeEditor->lineNumberAreaWidth(), 0);
	}

protected:
	//! Override default paintEvent
	void paintEvent(QPaintEvent *event) {
		codeEditor->lineNumberAreaPaintEvent(event);
	}

private:
	ScriptTextEdit *codeEditor;
};































#endif
