#ifndef SCRIPTITEM_H
#define SCRIPTITEM_H

#include <QSharedPointer>
#include <QTextEdit>
#include <QColor>
#include "diagramitem.h"
#include "../property/Property.h"
#include "ScriptSyntaxHighlighter.h"

/*! \brief A small box placed on arrow sources and destinations that represents a non-empty script.
 *	\details The box can be colored according to its function (we use blue for experiment scripts and
 *	yellow for analysis scripts, and is also outlinable if it contains a search string.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class ScriptItem : public DiagramItem
{
	Q_OBJECT
public:
	ScriptItem(QSharedPointer<Property> scriptProperty, QColor baseColor, QSharedPointer<EditorState> editorState,QGraphicsItem *parent);
	virtual ~ScriptItem();
protected:
	virtual void setRect(QRectF rect);
	virtual void updateDependantGraphics(){};
	virtual QVariant itemChange(GraphicsItemChange change, const QVariant & value);
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


#endif
