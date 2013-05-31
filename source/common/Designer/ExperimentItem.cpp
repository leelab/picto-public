#include "ExperimentItem.h"
#include "../../common/memleakdetect.h"
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

void ExperimentItem::hoverEnterEvent ( QGraphicsSceneHoverEvent *event )
{
	if(editorState_->getCurrentAnalysis().isNull())
		AssetItem::hoverEnterEvent(event);
}

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