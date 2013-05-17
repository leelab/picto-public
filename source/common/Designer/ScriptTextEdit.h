#ifndef SCRIPTTEXTEDIT_H
#define SCRIPTTEXTEDIT_H
#include <QEvent>
#include "SearchableTextEdit.h"
#include "ScriptSyntaxHighlighter.h"
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
// A Text editor widget for Picto Scripts
class ScriptTextEdit : public SearchableTextEdit
{
	Q_OBJECT
public: 
	ScriptTextEdit(bool singleLine = false);
protected:
	virtual void focusOutEvent(QFocusEvent *e);
private:
	ScriptSyntaxHighlighter *syntaxHighlighter_;
};
































//! [0]
#endif
