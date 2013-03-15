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

	//Set up search functionality
	//Set "search text found" highlight color
	setHighlightColor(SearchRequest::getGroupTypeIndex(SearchRequest::EXPERIMENT,SearchRequest::STRING),QColor(255,0,0));
	setHighlightColor(SearchRequest::getGroupTypeIndex(SearchRequest::ANALYSIS,SearchRequest::STRING),QColor(255,0,0));
	//Connect search signal to this object
	connect(editorState.data(),SIGNAL(searchRequested(SearchRequest)),this,SLOT(searchRequested(SearchRequest)));

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
	latestText_ = newValue.toString();
	QString newStringValue = latestText_;
	if(!newStringValue.isEmpty())
	{
		scriptTextEdit_.setText(newStringValue);
		newStringValue = scriptTextEdit_.document()->toHtml("utf-8");
	}
	
	setToolTip(newStringValue);

	//If any searches are already going on, trigger searchRequested now in case the edit affects the search
	foreach(SearchRequest searchRequest,editorState_->getSearchRequests())
	{
		searchRequested(searchRequest);
	}
}

void ScriptItem::searchRequested(SearchRequest searchRequest)
{
	if(searchRequest.type != SearchRequest::STRING)
		return;
	if(searchRequest.enabled && latestText_.contains(searchRequest.query,searchRequest.caseSensitive?Qt::CaseSensitive:Qt::CaseInsensitive))
		enableOutline(searchRequest.getGroupTypeIndex(),true);
	else
		enableOutline(searchRequest.getGroupTypeIndex(),false);
}