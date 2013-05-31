#include "AnalysisItem.h"
#include "../../common/statemachine/Result.h"
#include "../../common/memleakdetect.h"
AnalysisItem::AnalysisItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
AssetItem(editorState,contextMenu,asset)
{
	QSharedPointer<Result> result = asset.dynamicCast<Result>();
	Q_ASSERT(!result.isNull());
	setName(result->getName());

	setRect(QRectF(QPointF(-19,-30),QPointF(19,30)));
	setSvgIcon(":/icons/result.svg");
}