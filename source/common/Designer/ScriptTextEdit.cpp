#include <QPainter>
#include <QScrollBar>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

#include "ScriptTextEdit.h"

#include "../../common/memleakdetect.h"

/*! \brief Constructs a ScriptTextEdit with the input EditorState object and parent widget.
 */
ScriptTextEdit::ScriptTextEdit(bool singleLine) :
SearchablePlainTextEdit(singleLine)
{
	syntaxHighlighter_ = new ScriptSyntaxHighlighter(document());
	setLineWrapMode(QPlainTextEdit::NoWrap);
	setMinimumWidth(100);
	setTabStopWidth(24);

	lineNumberArea = new LineNumberArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

	if (!singleLine)
	{
		document()->setDefaultFont(QFont("Courier", 8));
	}

	updateLineNumberAreaWidth(0);
	highlightCurrentLine();
}

/*! \brief Extends SearchableTextEdit::focusOutEvent() to handle updating of this Text Editor when the ScriptTextEdit
 *	loses focus.
 *	\details Since some parts of Picto consider a script as "existing" so long as it contains any characters, when
 *	focusing out of a script text edit, if the box contains only white space, we just empty it of its contents such
 *	that the rest of Picto will know that it's empty.
 */
void ScriptTextEdit::focusOutEvent(QFocusEvent *e)
{
	//Since some parts of Picto consider a script as "existing" so long as it contains
	//any characters, when focusing out of a script text edit, if the box contains only
	//white space, we just empty it of its contents such that the rest of Picto will
	//know that it's empty.
	if (!toPlainText().isEmpty() && toPlainText().trimmed().isEmpty())
	{
		setPlainText("");
	}
	SearchablePlainTextEdit::focusOutEvent(e);
}

//! Sets the margins to fit the LineNumberArea widget
void ScriptTextEdit::updateLineNumberAreaWidth(int)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

//! Scrolls the LineNumberArea widget
void ScriptTextEdit::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
	{
		lineNumberArea->scroll(0, dy);
	}
	else
	{
		lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
	}

	if (rect.contains(viewport()->rect()))
	{
		updateLineNumberAreaWidth(0);
	}
}

//! Gets the desired width of the LineNumberArea Widget
int ScriptTextEdit::lineNumberAreaWidth() const
{
	if (singleLine_)
	{
		return 0;
	}

	int digits = 1;
	int max = qMax(1, document()->blockCount());
	while (max >= 10)
	{
		max /= 10;
		++digits;
	}

	int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

	return space;
}

//! Paints the LineNumberArea widget
void ScriptTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(lineNumberArea);
	painter.fillRect(event->rect(), Qt::lightGray);
	
	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int)blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom())
	{
		if (block.isVisible() && bottom >= event->rect().top())
		{
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int)blockBoundingRect(block).height();
		++blockNumber;
	}
}

//! Highlights the current line
void ScriptTextEdit::highlightCurrentLine()
{
	if (singleLine_)
		return;

	lineHighlight_.clear();

	if (!isReadOnly())
	{
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::yellow).lighter(160);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		lineHighlight_.append(selection);
	}

	applyExtraSelections();
}

//! Overriding default resizeEvent
void ScriptTextEdit::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

//! Set the ExtraSelections to include both SearchResults and LineHighlighting
void ScriptTextEdit::applyExtraSelections()
{
	setExtraSelections(lineHighlight_ + searchResults_);
}
