#include "AssetItem.h"
AssetItem::AssetItem(QMenu *itemMenu, QSharedPointer<Asset> asset) :
DiagramItem(
			itemMenu
			),
asset_(asset)
{
	QPolygonF myPolygon;
	myPolygon	<< QPointF(-100, -100) << QPointF(100, -100)
		  << QPointF(100, 100) << QPointF(-100, 100)
		  << QPointF(-100, -100);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setPolygon(myPolygon);

	QString name = asset->name();
	if(name == "")
		name = "NO NAME";
	setName(name,QPointF(-100,-100));
	QString type = asset->identifier();
	if(type == "")
		type = "UNDEFINED TYPE";
	setType(type);
}