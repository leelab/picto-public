#ifndef REWARDITEM_H
#define REWARDITEM_H

#include "StateMachineElementItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class RewardItem : public StateMachineElementItem
{
	Q_OBJECT
public:
	 RewardItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
private:

};
//! [0]

#endif
