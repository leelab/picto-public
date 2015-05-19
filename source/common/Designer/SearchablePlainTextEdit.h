#ifndef _SEARCHABLE_PLAIN_TEXT_EDIT_H
#define _SEARCHABLE_PLAIN_TEXT_EDIT_H
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QKeyEvent>
#include "../storage/SearchRequest.h"

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

/*! \brief A QPlainTextEdit that is searchable by using a SearchRequest object.
 *	\details When a SearchRequest is found in this QPlainTextEdit, the strings that are found will be highlighted.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class SearchablePlainTextEdit : public QPlainTextEdit
{
	Q_OBJECT
public: 
	SearchablePlainTextEdit(bool singleLine = false, QWidget *parent = NULL);
	void search(SearchRequest searchRequest, QColor highlightColor);
	QSize sizeHint() const;

signals:
	void focusOut();

protected:
	virtual void applyExtraSelections();
	void keyPressEvent(QKeyEvent * e);
	virtual bool event(QEvent* e);
	virtual void focusOutEvent(QFocusEvent *e);

	bool singleLine_;

	QList<QTextEdit::ExtraSelection> searchResults_;
};































#endif
