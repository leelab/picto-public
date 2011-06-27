#ifndef STATEITEM_H
#define STATEITEM_H

#include "StateMachineElementItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class StateItem : public StateMachineElementItem
{
	Q_OBJECT
public:
	 StateItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);

};
//! [0]

#endif
