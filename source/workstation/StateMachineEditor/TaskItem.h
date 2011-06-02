#ifndef TASKITEM_H
#define TASKITEM_H

#include "AssetItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class TaskItem : public AssetItem
{
	Q_OBJECT
public:
	 TaskItem(QMenu *itemMenu, QSharedPointer<Asset> asset);
	 QSharedPointer<Asset> getAsset(){return asset_;};
private:
	QSharedPointer<Asset> asset_;

};
//! [0]

#endif
