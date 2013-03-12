#include "WireableResultItem.h"
#include "../../common/statemachine/Result.h"
#include "../../common/memleakdetect.h"
WireableResultItem::WireableResultItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QMenu *scriptContextMenu, QSharedPointer<Asset> asset) :
WireableItem(editorState,contextMenu,scriptContextMenu,asset)
{
	QSharedPointer<Result> result = asset.dynamicCast<Result>();
	Q_ASSERT(!result.isNull());
	setName(result->getName());
	enableArrowDest();

	setRect(QRectF(QPointF(-19,-30),QPointF(19,30)));
	setSvgIcon(":/icons/result.svg");
}