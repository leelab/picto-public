#include "AssetDiagramItem.h"
#include "../../common/storage/datastore.h"
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
	QSharedPointer<DataStore> dataStore = asset.staticCast<DataStore>();
	if(!dataStore.isNull())
	{
		QList<QSharedPointer<Asset>> results = dataStore->getGeneratedChildren("Result");
		foreach(QSharedPointer<Asset> result,results)
		{
			QSharedPointer<Property> resultProp = result.staticCast<Property>();
			if(!resultProp.isNull())
				addArrowSource(resultProp->value().toString());
		}
		enableArrowDest();
	}
}