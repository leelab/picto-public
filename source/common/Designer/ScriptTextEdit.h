#ifndef SCRIPTTEXTEDIT_H
#define SCRIPTTEXTEDIT_H
#include <QTextEdit>
#include <QEvent>
#include "ScriptSyntaxHighlighter.h"
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
// A Text editor widget for Picto Scripts
class ScriptTextEdit : public QTextEdit
{
	Q_OBJECT
public: 
	ScriptTextEdit();
signals:
	void focusOut();
protected:
	virtual bool event(QEvent* e);
	virtual void focusOutEvent(QFocusEvent *e);
private:
	ScriptSyntaxHighlighter *syntaxHighlighter_;
};
































//! [0]
#endif
