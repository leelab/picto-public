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
	 StateMachineElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);

};
//! [0]

#endif
