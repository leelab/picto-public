#ifndef STATEMACHINEELEMENTITEM_H
#define STATEMACHINEELEMENTITEM_H

#include "WireableItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class StateMachineElementItem : public WireableItem
{
	Q_OBJECT
public:
	 StateMachineElementItem(QMenu *itemMenu, QSharedPointer<Asset> asset);
	 QSharedPointer<Asset> getAsset(){return asset_;};
private:
	QSharedPointer<Asset> asset_;

};
//! [0]

#endif
