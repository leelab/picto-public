#include "StateMachineItem.h"
StateMachineItem::StateMachineItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
StateMachineElementItem(editorState,contextMenu,asset)
{
}