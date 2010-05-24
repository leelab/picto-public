/*! \brief A text editor for editing experiment xml
 *
 *	This is a variant on the standard QText Edit.  The following features should
 *	be added:
 *		1. Context menu: "Jump to matching tag"
 *		2. ContextMenu: "Check syntax"
 *		3. Syntax highlighting?
 *		4. Outlining?
 */

#ifndef _XML_EDITOR_H_
#define _XML_EDITOR_H_

#include <QTextEdit>
#include <QList>
#include <QTextCursor>

class QAction; 

class XmlEditor : public QTextEdit
{
	Q_OBJECT
public:
	XmlEditor(QWidget *parent = 0);

	void addContextMenuAction(QAction *action);

public slots:
	void goToMatchingTag();

protected:
	void contextMenuEvent(QContextMenuEvent *ev);

private:
	void createActions();
	bool isStartTag(const QTextCursor curs);
	bool isInTag(const QTextCursor curs);

	QAction *goToMatchAction_;
	QList<QAction*> contextActions_;
	QPoint contextMenuGlobalPos_;

};

#endif