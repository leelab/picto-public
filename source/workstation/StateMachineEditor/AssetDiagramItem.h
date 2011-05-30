#ifndef ASSETDIAGRAMITEM_H
#define ASSETDIAGRAMITEM_H

#include "diagramitem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class AssetDiagramItem : public DiagramItem
{
	Q_OBJECT
public:
	 AssetDiagramItem(QMenu *itemMenu, DiagramType diagramType, QSharedPointer<Asset> asset);
	 QSharedPointer<Asset> getAsset(){return asset_;};
private:
	QSharedPointer<Asset> asset_;

};
//! [0]

#endif
