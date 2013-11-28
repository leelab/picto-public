#include "AnalysisItem.h"
#include "../../common/memleakdetect.h"
/*! \brief Constructs a new AnalysisItem.  Inputs are passed directly to the AssetItem::AssetItem.*/
AnalysisItem::AnalysisItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
AssetItem(editorState,contextMenu,asset)
{
}

void AnalysisItem::setRect(QRectF rect)
{
	QLinearGradient grad(QPointF(0,-20),QPointF(0,20));
	float stopPoint = (getIconRect().top()-getRect().top())/getRect().height();
	QColor startColor("#4f4f00");
	startColor = startColor.darker();
	QColor stopColor("#777700");
	grad.setColorAt(0,startColor);
	grad.setColorAt(stopPoint,stopPoint<1?stopColor:startColor);
	QBrush brush(grad);
	setBrush(brush);
	AssetItem::setRect(rect);
}