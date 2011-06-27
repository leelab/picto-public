#include "StateItem.h"
StateItem::StateItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
StateMachineElementItem(editorState,contextMenu,asset)
{
}