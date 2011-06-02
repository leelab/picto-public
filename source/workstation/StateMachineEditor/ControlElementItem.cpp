#include "ControlElementItem.h"
#include "../../common/ControlElements/ControlElement.h"
ControlElementItem::ControlElementItem(QMenu *itemMenu, QSharedPointer<Asset> asset) :
WireableItem(itemMenu,asset)
{
	QSharedPointer<ControlElement> ctrlElem = asset.staticCast<ControlElement>();
	QStringList results = ctrlElem->getResultList();
	foreach(QString result,results)
	{
		addArrowSource(result);
	}
}