#include "AssetDiagramItem.h"
#include "../../common/storage/datastore.h"
#include "../../common/statemachine/statemachineelement.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
AssetDiagramItem::AssetDiagramItem(QMenu *itemMenu, DiagramType diagramType, QSharedPointer<Asset> asset) :
DiagramItem(
			(asset && (asset->assetType() == "DataStore"))?Step:Conditional,
			itemMenu
			),
asset_(asset)
{
	QString name = asset->name();
	if(name == "")
		name = "NO NAME";
	setName(name);
	QString type = asset->identifier();
	if(type == "")
		type = "UNDEFINED TYPE";
	setType(type);
	QSharedPointer<StateMachineElement> stateMachElem = asset.dynamicCast<StateMachineElement>();
	if(!stateMachElem.isNull())
	{
		QStringList results = stateMachElem->getResultList();
		foreach(QString result,results)
		{
			addArrowSource(result);
		}
		enableArrowDest();
	}
}