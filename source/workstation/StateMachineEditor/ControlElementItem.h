#ifndef CONTROLELEMENTITEM_H
#define CONTROLELEMENTITEM_H
#include "WireableItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class ControlElementItem : public WireableItem
{
	Q_OBJECT
public:
	 ControlElementItem(QMenu *itemMenu, QSharedPointer<Asset> asset);
	 QSharedPointer<Asset> getAsset(){return asset_;};
private:
	QSharedPointer<Asset> asset_;

};
//! [0]

#endif
