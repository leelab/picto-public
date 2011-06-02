#include "ResultItem.h"
#include "../../common/property/Property.h"
ResultItem::ResultItem(QMenu *itemMenu, QSharedPointer<Asset> asset) :
WireableItem(itemMenu,asset)
{
	QSharedPointer<Property> prop = asset.dynamicCast<Property>();
	Q_ASSERT(!prop.isNull());
	setName(prop->value().toString());
	enableArrowDest();
}