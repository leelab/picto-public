#include "AssetDiagramItem.h"
#include "../../common/storage/datastore.h"
#include "../../common/statemachine/statemachineelement.h"
#include "../../common/controlelements/controlelement.h"
#include "../../common/property/property.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
AssetDiagramItem::AssetDiagramItem(QMenu *itemMenu, DiagramType diagramType, QSharedPointer<Asset> asset) :
DiagramItem(
			Step,
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
	else
	{
		QSharedPointer<ControlElement> ctrlElem = asset.dynamicCast<ControlElement>();
		if(!ctrlElem.isNull())
		{
			QStringList results = ctrlElem->getResultList();
			foreach(QString result,results)
			{
				addArrowSource(result);
			}
			enableArrowDest();
		}
		else
		{
			QSharedPointer<Property> prop = asset.dynamicCast<Property>();
			if(!prop.isNull())
			{
				setName(prop->value().toString());
				enableArrowDest();
			}
		}
	}
}