#ifndef ASSETITEM_H
#define ASSETITEM_H

#include "DiagramItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class AssetItem : public DiagramItem
{
	Q_OBJECT
public:
	 AssetItem(QMenu *itemMenu, QSharedPointer<Asset> asset);
	 QSharedPointer<Asset> getAsset(){return asset_;};
	 virtual void removeDependantGraphics(){};
protected:
	virtual void updateDependantGraphics(){};
private:
	QSharedPointer<Asset> asset_;

};
//! [0]

#endif
