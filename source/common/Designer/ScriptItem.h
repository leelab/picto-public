#ifndef SCRIPTITEM_H
#define SCRIPTITEM_H

#include <QSharedPointer>
#include <QTextEdit>
#include <QColor>
#include "diagramitem.h"
#include "../property/Property.h"
#include "ScriptSyntaxHighlighter.h"

//! [0]
class ScriptItem : public DiagramItem
{
	Q_OBJECT
public:
	ScriptItem(QSharedPointer<Property> scriptProperty, QColor baseColor, QSharedPointer<EditorState> editorState,QGraphicsItem *parent);
	virtual ~ScriptItem();
protected:
	virtual void setRect(QRectF rect);
	virtual void updateDependantGraphics(){};
private:
	QSharedPointer<Property> scriptProperty_;
	ScriptSyntaxHighlighter* scriptSyntaxHighlighter_;		//These are used for syntax highlighting
	QTextEdit scriptTextEdit_;								//of scripts
	QColor baseColor_;
	QString latestText_;
private slots:
	void scriptPropEdited(Property* changedProp,QVariant newValue);
	void searchRequested(SearchRequest searchRequest);
};
//! [0]

#endif
