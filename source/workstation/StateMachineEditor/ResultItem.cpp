#include "ResultItem.h"
#include "../../common/statemachine/Result.h"
ResultItem::ResultItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
WireableItem(editorState,contextMenu,asset)
{
	QSharedPointer<Result> result = asset.dynamicCast<Result>();
	Q_ASSERT(!result.isNull());
	setName(result->getName());
	enableArrowDest();
}