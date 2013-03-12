#include "StateMachineElementItem.h"
#include "../../common/StateMachine/StateMachineElement.h"
#include "../../common/memleakdetect.h"
StateMachineElementItem::StateMachineElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QMenu *scriptContextMenu, QSharedPointer<Asset> asset) :
WireableItem(editorState,contextMenu,scriptContextMenu,asset)
{
	QSharedPointer<StateMachineElement> stateMachElem = asset.staticCast<StateMachineElement>();
	QList<QSharedPointer<Asset>> results = stateMachElem->getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> result,results)
	{
		addArrowSource(result);
	}
	enableArrowDest();

	//Set default size and icon
	setRect(QRectF(QPointF(-19,-30),QPointF(19,30)));
	setSvgIcon(":/icons/statemachineelement.svg");
}