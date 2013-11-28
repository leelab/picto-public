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

/*! \brief A Text Editor widget for Picto scripts.
 *	\details This is the widget used in the Picto Property window in the Designer
 *	for editing Picto scripts.  It includes syntax highlighting based on the
 *	ScriptSyntaxHighlighter and is searchable such that the searched term is 
 *	highlighted.
 *	\note This widget is used by the ScriptWidget which handles search commands and some
 *	other functionality.
 *	\sa ScriptWidget
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
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

































#endif
