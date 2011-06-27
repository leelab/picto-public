#ifndef FLOWELEMENTITEM_H
#define FLOWELEMENTITEM_H

#include "StateMachineElementItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class FlowElementItem : public StateMachineElementItem
{
	Q_OBJECT
public:
	FlowElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
private:

};
//! [0]

#endif
