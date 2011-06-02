#ifndef FLOWELEMENTITEM_H
#define FLOWELEMENTITEM_H

#include "StateMachineElementItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class FlowElementItem : public StateMachineElementItem
{
	Q_OBJECT
public:
	FlowElementItem(QMenu *itemMenu, QSharedPointer<Asset> asset);
	QSharedPointer<Asset> getAsset(){return asset_;};
private:
	QSharedPointer<Asset> asset_;

};
//! [0]

#endif
