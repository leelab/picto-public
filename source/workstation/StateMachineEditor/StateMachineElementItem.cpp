#include "StateMachineElementItem.h"
#include "../../common/StateMachine/StateMachineElement.h"
StateMachineElementItem::StateMachineElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
WireableItem(editorState,contextMenu,asset)
{
	QSharedPointer<StateMachineElement> stateMachElem = asset.staticCast<StateMachineElement>();
	QList<QSharedPointer<Asset>> results = stateMachElem->getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> result,results)
	{
		addArrowSource(result->getName());
	}
	enableArrowDest();
}