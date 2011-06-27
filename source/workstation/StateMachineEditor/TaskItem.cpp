#include "TaskItem.h"
TaskItem::TaskItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
AssetItem(editorState,contextMenu,asset)
{
}