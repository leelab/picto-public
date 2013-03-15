#ifndef SEARCHABLETEXTEDIT_H
#define SEARCHABLETEXTEDIT_H
#include <QTextEdit>
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
	SearchableTextEdit(QWidget *parent=NULL);
	void search(SearchRequest searchRequest, QColor highlightColor);
};
































//! [0]
#endif
