#include "ScriptTextEdit.h"
#include "../../common/memleakdetect.h"

ScriptTextEdit::ScriptTextEdit(bool singleLine) :
SearchableTextEdit(singleLine)
{
	syntaxHighlighter_ = new ScriptSyntaxHighlighter(document());
	setLineWrapMode(QTextEdit::NoWrap);
	setMinimumWidth(100);
	setTabStopWidth(16);
}

void ScriptTextEdit::focusOutEvent(QFocusEvent *e)
{
	//Since some parts of Picto consider a script as "existing" so long as it contains
	//any characters, when focusing out of a script text edit, if the box contains only
	//white space, we just empty it of its contents such that the rest of Picto will
	//know that it's empty.
	if(toPlainText().trimmed().isEmpty())
		setText("");
	SearchableTextEdit::focusOutEvent(e);
}