#include <QtGui>
#include <QStyleOption>
#include <QApplication>

#include "SearchableTextEdit.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a SearchableTextEdit with the input parent.
 *	\details If singleLine is set true, the text edit will contain one line, otherwise it will be a regular scrollable
 *	QPlainTextEdit.
 */
SearchableTextEdit::SearchableTextEdit(bool singleLine,QWidget *parent) :
QTextEdit(parent),
singleLine_(singleLine)
{
	if(singleLine_)
	{
		setTabChangesFocus(true);
        setWordWrapMode(QTextOption::NoWrap);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setFixedHeight(sizeHint().height());
	}
}

/*! \brief Searches the QTextEdit for the input SearchRequest.  If found, strings matching the SearchRequest are
 *	highlighted with the input highlightColor.
 */
void SearchableTextEdit::search(SearchRequest searchRequest, QColor highlightColor)
{
    QList<ExtraSelection> extraSelections;
	if(!searchRequest.enabled)
	{
		return setExtraSelections(extraSelections);
	}
	QTextCursor startingPos = textCursor();
    moveCursor(QTextCursor::Start);
    QColor color = highlightColor;
 
	ExtraSelection extra;
	extra.format.setBackground(QBrush(color));
    while	(
				(searchRequest.caseSensitive && find(searchRequest.query,QTextDocument::FindCaseSensitively)) ||
				(!searchRequest.caseSensitive && find(searchRequest.query))
			
			)
    {
        extra.cursor = textCursor();
        extraSelections.append(extra);
    }
	setExtraSelections(extraSelections);
	setTextCursor(startingPos);
}

/*! \brief This extends QTextEdit::sizeHint to make the QTextEdit act like a QLineEdit in case this object was
 *	constructed with singleLine.
 *	\details The code that turns a QTextEdit into a QLineEdit is from
 *	http://qt-project.org/faq/answer/how_can_i_create_a_one-line_qtextedit
 */
QSize SearchableTextEdit::sizeHint() const
{
	if(!singleLine_)
		return QTextEdit::sizeHint();
	QFontMetrics fm(font());
	int h = qMax(fm.height(), 14) + 4;
	int w = fm.width(QLatin1Char('x')) * 17 + 4;
	QStyleOptionFrameV2 opt;
	opt.initFrom(this);
	return (style()->sizeFromContents(QStyle::CT_LineEdit, &opt, QSize(w, h).expandedTo(QApplication::globalStrut()), this));
}

/*! \brief This extends QTextEdit::keyPressEvent to make the QTextEdit act like a QLineEdit in case this object
 *	was constructed with singleLine.
 *	\details The code that turns a QTextEdit into a QLineEdit is from
 *	http://qt-project.org/faq/answer/how_can_i_create_a_one-line_qtextedit
 */
void SearchableTextEdit::keyPressEvent(QKeyEvent * e)
{
	if (singleLine_ && (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter))
		e->ignore();
	else
		QTextEdit::keyPressEvent(e);
}

/*! \brief This extends QTextEdit::event to disable tooltips. */
bool SearchableTextEdit::event(QEvent* e)
{
	//Don't show tooltips.
	if(e->type() == QEvent::ToolTip)
		return true;
	return QTextEdit::event(e);
}

/*! \brief This extends QTextEdit::focusOutEvent() so that we can emit focusOut() as a signal whenever the
 *	QTextEdit loses focus.
 */
void SearchableTextEdit::focusOutEvent(QFocusEvent *e)
{
	QTextEdit::focusOutEvent(e);
	emit focusOut();
}