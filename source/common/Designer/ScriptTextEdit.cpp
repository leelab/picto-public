#include "ScriptTextEdit.h"
#include "../../common/memleakdetect.h"

ScriptTextEdit::ScriptTextEdit()
{
	syntaxHighlighter_ = new ScriptSyntaxHighlighter(document());
	setLineWrapMode(QTextEdit::NoWrap);
	setMinimumWidth(100);
	setTabStopWidth(16);
}

bool ScriptTextEdit::event(QEvent* e)
{
	if(e->type() == QEvent::ToolTip)
		return true;
	return QTextEdit::event(e);
}


void ScriptTextEdit::focusOutEvent(QFocusEvent *e)
{
	QTextEdit::focusOutEvent(e);

	//Since some parts of Picto consider a script as "existing" so long as it contains
	//any characters, when focusing out of a script text edit, if the box contains only
	//white space, we just empty it of its contents such that the rest of Picto will
	//know that it's empty.
	if(toPlainText().trimmed().isEmpty())
		setText("");
	emit focusOut();
}