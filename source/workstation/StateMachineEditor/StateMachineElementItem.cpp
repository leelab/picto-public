#include "StateMachineElementItem.h"
#include "../../common/StateMachine/StateMachineElement.h"
StateMachineElementItem::StateMachineElementItem(QMenu *itemMenu, QSharedPointer<Asset> asset) :
WireableItem(itemMenu,asset)
{
	QSharedPointer<StateMachineElement> stateMachElem = asset.staticCast<StateMachineElement>();
	QStringList results = stateMachElem->getResultList();
	foreach(QString result,results)
	{
		addArrowSource(result);
	}
	enableArrowDest();
}