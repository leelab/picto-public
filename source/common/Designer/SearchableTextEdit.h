#ifndef SEARCHABLETEXTEDIT_H
#define SEARCHABLETEXTEDIT_H
#include <QTextEdit>
#include <QKeyEvent>
#include "../storage/SearchRequest.h"

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
// A Text editor widget for Picto Scripts
class SearchableTextEdit : public QTextEdit
{
	Q_OBJECT
public: 
	SearchableTextEdit(bool singleLine = false,QWidget *parent=NULL);
	void search(SearchRequest searchRequest, QColor highlightColor);
	QSize sizeHint() const;
signals:
	void focusOut();
protected:
	void keyPressEvent(QKeyEvent * e);
	virtual bool event(QEvent* e);
	virtual void focusOutEvent(QFocusEvent *e);
private:
	bool singleLine_;
};
































//! [0]
#endif
