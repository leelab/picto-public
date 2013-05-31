#ifndef AnalysisItem_H
#define AnalysisItem_H

#include "AssetItem.h"
#include "../../common/storage/asset.h"
#include <QSharedPointer>
using namespace Picto;

//! [0]
class AnalysisItem : public AssetItem
{
	Q_OBJECT
public:
	 AnalysisItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	 virtual ~AnalysisItem(){};

};
//! [0]

#endif
