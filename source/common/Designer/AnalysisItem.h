#ifndef AnalysisItem_H
#define AnalysisItem_H

#include <QSharedPointer>
#include <QGraphicsOpacityEffect>
#include "AssetItem.h"
#include "../parameter/Analysis.h"
#include "../../common/storage/asset.h"

using namespace Picto;

//! [0]
class AnalysisItem : public AssetItem
{
	Q_OBJECT
public:
	AnalysisItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	virtual ~AnalysisItem(){};

protected:
	virtual void setRect(QRectF rect);

};
//! [0]

#endif
