#include "RewardItem.h"
RewardItem::RewardItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
StateMachineElementItem(editorState,contextMenu,asset)
{
}