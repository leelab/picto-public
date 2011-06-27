#ifndef TASKITEM_H
#define TASKITEM_H

#include "AssetItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class TaskItem : public AssetItem
{
	Q_OBJECT
public:
	 TaskItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);

};
//! [0]

#endif
