#include <QGraphicsScene>
#include <QToolTip>
#include <QApplication>
#include "ScriptItem.h"
#include "ArrowDestinationItem.h"
#include "Arrow.h"
#include "../../common/memleakdetect.h"
/*! \brief Constructs a new ScriptItem for the input scriptProperty with the input baseColor.  Other inputs are passed directly to the DiagramItem constructor.
*/
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
	setHighlightColor(SearchRequest::getGroupTypeIndex(SearchRequest::ACTIVE_ANALYSES,SearchRequest::STRING),QColor(255,0,0));
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
	//If this is an Experimental script and there's an active analysis, decrease its alpha.
	//I know... this object shouldn't know anything about whether its an experimental or analysis script...
	//We're on a tight schedule.  :(
	bool isExperimental = (baseColor_ == Qt::blue);
	QColor startColor(baseColor_.lighter());
	QColor stopColor(baseColor_.darker());
	if(isExperimental && !editorState_->getCurrentAnalysis().isNull())
	{
		startColor.setAlpha(100);
		stopColor.setAlpha(100);
	}
	grad.setColorAt(0,startColor);
	grad.setColorAt(1,stopColor);
	QBrush brush(grad);
	setBrush(brush);
}

/*! \brief Disables the itemChange function of DiagramItem since ScriptItems can't be independently
 *	moved or become WindowAssets.
 */
QVariant ScriptItem::itemChange(GraphicsItemChange change, const QVariant & value)
{
	//QVariant returnVal = value;
	////Intercept the tooltip before it's created and change its properties
	//if(change == QGraphicsItem::ItemToolTipChange)
	//{
	//	QToolTip* theTip = (QToolTip *) value.value<void *>();
	//	theTip->
	//	return qVariantFromValue((void *) theTip);
	//}
	return QGraphicsItem::itemChange(change, value);

}
/*! \brief Called when the script Property represented by this ScriptItem is edited to update its script contents tooltip and search highlighting.
*/
void ScriptItem::scriptPropEdited(Property*,QVariant newValue)
{
	latestText_ = newValue.toString();
	QString newStringValue = latestText_;
	if(!newStringValue.isEmpty())
	{
		scriptTextEdit_.setText(newStringValue);
		newStringValue = scriptSyntaxHighlighter_->asHtml();
	}
	
	setToolTip(newStringValue);

	//If any searches are already going on, trigger searchRequested now in case the edit affects the search
	foreach(SearchRequest searchRequest,editorState_->getSearchRequests())
	{
		searchRequested(searchRequest);
	}
}

/*! \brief Called when a search is requested to highlight the ScriptItem if its script contents match the search.
*/
void ScriptItem::searchRequested(SearchRequest searchRequest)
{
	if(searchRequest.type != SearchRequest::STRING)
		return;
	if(searchRequest.enabled && latestText_.contains(searchRequest.query,searchRequest.caseSensitive?Qt::CaseSensitive:Qt::CaseInsensitive))
		enableOutline(searchRequest.getGroupTypeIndex(),true);
	else
		enableOutline(searchRequest.getGroupTypeIndex(),false);
}