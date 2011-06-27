#ifndef SCRIPTELEMENTITEM_H
#define SCRIPTELEMENTITEM_H

#include "StateMachineElementItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class ScriptElementItem : public StateMachineElementItem
{
	Q_OBJECT
public:
	 ScriptElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
private:

};
//! [0]

#endif
