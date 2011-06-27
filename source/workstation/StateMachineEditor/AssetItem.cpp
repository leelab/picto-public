#include "AssetItem.h"
#include "../../common/property/property.h"
#include "../../common/statemachine/uienabled.h"
AssetItem::AssetItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
DiagramItem(editorState,contextMenu),
asset_(asset),
posChanged_(false)
{
	QPolygonF myPolygon;
	myPolygon	<< QPointF(-100, -100) << QPointF(100, -100)
		  << QPointF(100, 100) << QPointF(-100, 100)
		  << QPointF(-100, -100);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setPolygon(myPolygon);
	assetEdited();
	connect(asset_.data(),SIGNAL(edited()),this,SLOT(assetEdited()));
}

AssetItem::~AssetItem()
{
}

void AssetItem::assetEdited()
{
	QString name = asset_->getName();
	if(name == "")
		name = "NO NAME";
	setName(name,QPointF(-100,-100));
	QString type = asset_->assetType();
	if(type == "")
		type = "UNDEFINED TYPE";
	setType(type);
}

void AssetItem::positionChanged(QPoint pos)
{
	if(asset_.isNull())
		return;
	asset_.staticCast<UIEnabled>()->setPos(pos);
	posChanged_ = true;
}

void AssetItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if(posChanged_)
	{
		if(!getEditorState().isNull())
			getEditorState()->setLastActionUndoable();
		posChanged_ = false;
	}
	QGraphicsItem::mouseReleaseEvent(event);
}