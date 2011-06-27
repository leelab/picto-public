#ifndef ASSETITEM_H
#define ASSETITEM_H

#include "DiagramItem.h"
#include "../../common/storage/asset.h"

#include "EditorState.h"
using namespace Picto;

//! [0]
class AssetItem : public DiagramItem
{
	Q_OBJECT
public:
	 AssetItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	 virtual ~AssetItem();
	 QSharedPointer<Asset> getAsset(){return asset_;};
public slots:
	void assetEdited();
protected:
	virtual void updateDependantGraphics(){};
	virtual void positionChanged(QPoint pos);
	virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent *event);
private:
	QSharedPointer<Asset> asset_;
	bool posChanged_;

};
//! [0]

#endif
