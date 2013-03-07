#ifndef WIREABLERESULTITEM_H
#define WIREABLERESULTITEM_H

#include "WireableItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class WireableResultItem : public WireableItem
{
	Q_OBJECT
public:
	 WireableResultItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	 virtual ~WireableResultItem(){};

};
//! [0]

#endif
