#include "ScriptElementItem.h"
ScriptElementItem::ScriptElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
StateMachineElementItem(editorState,contextMenu,asset)
{
}