#include "ExperimentItem.h"
#include "../../common/memleakdetect.h"
/*! \brief Constructs a new ExperimentItem.  Inputs are passed directly to the AssetItem::AssetItem.*/
ExperimentItem::ExperimentItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
AssetItem(editorState,contextMenu,asset)
{
	//opacityEffect_ = new QGraphicsOpacityEffect();
	currentAnalysisChanged(editorState_->getCurrentAnalysis());
}

void ExperimentItem::setRect(QRectF rect)
{
	applyIconOpacity();

	QLinearGradient grad(QPointF(0,-20),QPointF(0,20));
	float stopPoint = (getIconRect().top()-getRect().top())/getRect().height();
	QColor startColor("darkslategrey");
	QColor stopColor("lightslategrey");
	//Grey out this item if we're in "Analysis design" mode
	if(!editorState_->getCurrentAnalysis().isNull())
	{
		startColor.setAlpha(100);
		stopColor.setAlpha(100);
	}
	grad.setColorAt(0,startColor);
	grad.setColorAt(stopPoint,stopPoint<1?stopColor:startColor);
	//grad.setColorAt(1,QColor("red"));
	QBrush brush(grad);
	setBrush(brush);
	AssetItem::setRect(rect);
}

/*! \brief Stops the AssetItem from handling hoverEnterEvents if we're in "Analysis Design" mode, when
 *	experiment design operations are not allowed.
 */
void ExperimentItem::hoverEnterEvent ( QGraphicsSceneHoverEvent *event )
{
	if(editorState_->getCurrentAnalysis().isNull())
		AssetItem::hoverEnterEvent(event);
}

/*! \brief Called when the current Analysis changes to enable/disable/alter certain DiagramItem characteristics based on
 *	whether we are operating in "Analysis Design" or "ExperimentDesign" mode.
 *	\note It seems like this should have been connected to the EditorState::currentAnalysisChanged() signal, but I am 
 *	not going to change this now.  Its worth looking into though.
 */
void ExperimentItem::currentAnalysisChanged(QSharedPointer<Analysis> currAnalysis)
{
	setFlag(QGraphicsItem::ItemIsMovable,currAnalysis.isNull());
	if(currAnalysis.isNull())
	{
		setHighlightColor(SearchRequest::getGroupTypeIndex(SearchRequest::EXPERIMENT,SearchRequest::SCRIPT),QColor(0,0,255,220));
	}
	else
	{
		setHighlightColor(SearchRequest::getGroupTypeIndex(SearchRequest::EXPERIMENT,SearchRequest::SCRIPT),QColor(0,0,255,100));
	}
	applyIconOpacity();
}

/*! \brief This function doesn't do anything and should probably be removed.
*/
void ExperimentItem::applyIconOpacity()
{
	////Make sure the opacity effect is installed on the svg icon
	//QGraphicsItem* svgItem = getSvgItem();
	//if(svgItem && (svgItem->graphicsEffect() != opacityEffect_))
	//{
	//	svgItem->setGraphicsEffect(opacityEffect_);
	//}
	////Apply the icon opacity
	//if(editorState_->getCurrentAnalysis().isNull())
	//{
	//	opacityEffect_->setOpacity(1);
	//}
	//else
	//{
	//	opacityEffect_->setOpacity(100.0/255.0);
	//}
}