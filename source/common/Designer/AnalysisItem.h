#ifndef AnalysisItem_H
#define AnalysisItem_H

#include <QSharedPointer>
#include <QGraphicsOpacityEffect>
#include "AssetItem.h"
#include "../parameter/Analysis.h"
#include "../../common/storage/asset.h"

using namespace Picto;

/*! \brief An AssetItem used to represent Analysis elements.
 *	\details This is pretty much just an AssetItem that is colored yellowish to represent that
 *	it is part of an Analysis.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class AnalysisItem : public AssetItem
{
	Q_OBJECT
public:
	AnalysisItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	virtual ~AnalysisItem(){};

protected:
	virtual void setRect(QRectF rect);

};


#endif
