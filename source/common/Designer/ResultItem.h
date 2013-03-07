#ifndef RESULTITEM_H
#define RESULTITEM_H

#include "AssetItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class ResultItem : public AssetItem
{
	Q_OBJECT
public:
	 ResultItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	 virtual ~ResultItem(){};

};
//! [0]

#endif
