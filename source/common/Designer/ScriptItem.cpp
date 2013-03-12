#include <QGraphicsScene>
#include <QToolTip>
#include <QApplication>
#include "ScriptItem.h"
#include "ArrowDestinationItem.h"
#include "Arrow.h"
#include "../../common/memleakdetect.h"
ScriptItem::ScriptItem(QSharedPointer<Property> scriptProperty, QColor baseColor, QSharedPointer<EditorState> editorState,QGraphicsItem *parent) :
DiagramItem(editorState,NULL,"",parent)
{
	Q_ASSERT(scriptProperty);
	scriptProperty_ = scriptProperty;
	baseColor_ = baseColor;
	QPen invisiblePen;
	invisiblePen.setWidth(0);
	invisiblePen.setColor(QColor(0,0,0,0));
	setPen(invisiblePen);
	setFlag(QGraphicsItem::ItemIsSelectable,false);
	setFlag(QGraphicsItem::ItemIsMovable,false);

	//Set up syntax highlighting system to get rich highlighted script text for tooltips
	scriptTextEdit_.setVisible(false);
	scriptSyntaxHighlighter_ = new ScriptSyntaxHighlighter(scriptTextEdit_.document());

	//Call scriptEdited to initialize this object with values from the underlying property
	scriptPropEdited(0,scriptProperty_->value());
	connect(scriptProperty_.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(scriptPropEdited(Property*,QVariant)));
}

ScriptItem::~ScriptItem()
{
}

void ScriptItem::setRect(QRectF rect)
{
	if(toolTip().isEmpty())
		return;
	DiagramItem::setRect(rect);
	QLinearGradient grad(getRect().topLeft(),getRect().bottomLeft());
	grad.setColorAt(0,baseColor_.lighter());
	grad.setColorAt(1,baseColor_.darker());
	QBrush brush(grad);
	setBrush(brush);
}

void ScriptItem::scriptPropEdited(Property*,QVariant newValue)
{
	QString newStringValue = newValue.toString();
	if(!newStringValue.isEmpty())
	{
		scriptTextEdit_.setText(newStringValue);
		newStringValue = scriptTextEdit_.document()->toHtml("utf-8");
	}
	
	setToolTip(newStringValue);
}