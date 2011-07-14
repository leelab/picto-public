#ifndef CONTROLELEMENTITEM_H
#define CONTROLELEMENTITEM_H
#include "WireableItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class ControlElementItem : public WireableItem
{
	Q_OBJECT
public:
	 ControlElementItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
private:

};
//! [0]

#endif