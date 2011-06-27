#ifndef RESULTITEM_H
#define RESULTITEM_H

#include "WireableItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class ResultItem : public WireableItem
{
	Q_OBJECT
public:
	 ResultItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);

};
//! [0]

#endif
