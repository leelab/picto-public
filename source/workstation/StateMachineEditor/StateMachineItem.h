#ifndef STATEMACHINEITEM_H
#define STATEMACHINEITEM_H

#include "StateMachineElementItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class StateMachineItem : public StateMachineElementItem
{
	Q_OBJECT
public:
	 StateMachineItem(QMenu *itemMenu, QSharedPointer<Asset> asset);
	 QSharedPointer<Asset> getAsset(){return asset_;};
private:
	QSharedPointer<Asset> asset_;

};
//! [0]

#endif
