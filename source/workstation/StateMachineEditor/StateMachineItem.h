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
	 StateMachineItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);

};
//! [0]

#endif
