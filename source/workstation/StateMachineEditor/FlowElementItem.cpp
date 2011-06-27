#include "FlowElementItem.h"
FlowElementItem::FlowElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
StateMachineElementItem(editorState,contextMenu,asset)
{
}