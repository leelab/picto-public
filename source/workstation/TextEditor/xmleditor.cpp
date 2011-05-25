#include "xmleditor.h"

#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <QCursor>


XmlEditor::XmlEditor(QWidget *parent) :
	QTextEdit(parent)
{
	setAcceptRichText(false);
	setAutoFormatting(QTextEdit::AutoNone);
	setLineWrapMode(QTextEdit::NoWrap);

	createActions();
}

void XmlEditor::createActions()
{
	goToMatchAction_ = new QAction("&Go to matching tag", this);
	connect(goToMatchAction_,SIGNAL(triggered()), this, SLOT(goToMatchingTag()));

}

void XmlEditor::contextMenuEvent(QContextMenuEvent *ev)
{
	QMenu *menu = createStandardContextMenu();

	menu->addSeparator();
	menu->addAction(goToMatchAction_);

	menu->addSeparator();
	foreach(QAction* action, contextActions_)
	{
		menu->addAction(action);
	}

	contextMenuGlobalPos_ = ev->globalPos();
	menu->exec(ev->globalPos());

	delete menu;
}

void XmlEditor::addContextMenuAction(QAction *action)
{
	contextActions_.append(action);
}


/*!	\brief Jumps to the matching xml tag
 *
 *	The tag at the cursor (if it exists) is read, and the cursor is then
 *	jumped to the matching open/close tag.  This is kind of ugly, and I wonder
 *	like I could do a better job with the XML functionality built into Qt.
 *	However, QXmlStreamReader doesn't allow us to move backwards through the text
 *	and I don't want to take the time to write a DOM parser.
 */
void XmlEditor::goToMatchingTag()
{
	QTextCursor curs = cursorForPosition(mapFromGlobal(contextMenuGlobalPos_));
	QTextCursor initCurs = textCursor();
	
	if(!isInTag(curs))
		return;

	//Move to the beginning of the tag (since this is an XML document, we can assume
	//that this will move us to the beginning of some tag, even if the user isn't in a tag)
	curs = document()->find("<",curs,QTextDocument::FindBackward);
	if(curs.isNull())
		return;

	//Check for empty tag (e.g. <Task/> or <Task name="null task"/>)
	curs = document()->find(">",curs);
	curs.movePosition(QTextCursor::PreviousCharacter);
	curs.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
	if(curs.selectedText() == "/")
	{
		curs = document()->find("<",curs,QTextDocument::FindBackward);
		curs.clearSelection();
		curs.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
		setTextCursor(curs);
		return;

	}
	bool foundIt = false;

	curs = document()->find("<",curs,QTextDocument::FindBackward);
	curs.clearSelection();
	if(!isStartTag(curs))
		curs.movePosition(QTextCursor::NextCharacter);
	curs.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
	QString name = curs.selectedText();
		
	//Search for an end tag
	if(isStartTag(curs))
	{
		int nestedTags = 0;

		while(!foundIt)
		{
			curs = document()->find(name,curs,QTextDocument::FindWholeWords);

			if(curs.isNull())
				break;

			if(!isInTag(curs))
				continue;

			if(isStartTag(curs))
				nestedTags++;
			else
				if(nestedTags == 0)
					foundIt = true;
				else
					nestedTags--;
		}
	}
	else
	{
		int nestedTags = 0;

		while(!foundIt)
		{
			curs = document()->find(name,curs, QTextDocument::FindBackward | QTextDocument::FindWholeWords);

			if(curs.isNull())
				break;

			if(!isInTag(curs))
				continue;

			if(!isStartTag(curs))
				nestedTags++;
			else
				if(nestedTags == 0)
					foundIt = true;
				else
					nestedTags--;
		}

	}
	if(foundIt)
	{
		setTextCursor(curs);
		return;
	}
	else
	{
		setTextCursor(initCurs);
		return;
	}
}

bool XmlEditor::isStartTag(const QTextCursor curs)
{
	QTextCursor curs1 = curs;
	curs1 = document()->find("<",curs1,QTextDocument::FindBackward);
	curs1.clearSelection();
	curs1.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
	if(curs1.selectedText() == "/")
		return false;
	else 
		return true;

}

/*! \brief Returns true if the cursor is inside a tag
 *	
 *	Being "inside a tag" is defined as being enclosed by < and > with
 *	no other brackets
 */
bool XmlEditor::isInTag(const QTextCursor curs)
{
	QTextCursor curs1 = curs;
	QTextCursor curs2 = curs;

	//Check for a < to our left, then confirm that it comes before a > to the left
	curs1 = document()->find("<",curs,QTextDocument::FindBackward);
	curs2 = document()->find(">",curs,QTextDocument::FindBackward);
	if(curs1.isNull())
		return false;
	if(!curs2.isNull() && curs2 > curs1)
		return false;

	//Check for a > to our right, then confirm that it comes before a < to the rigth
	curs1 = document()->find(">",curs);
	curs2 = document()->find("<",curs);
	if(curs1.isNull())
		return false;
	if(!curs2.isNull() && curs2 < curs1)
		return false;
	return true;
}
