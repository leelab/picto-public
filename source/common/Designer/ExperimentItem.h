#ifndef ExperimentItem_H
#define ExperimentItem_H

#include <QSharedPointer>
#include <QGraphicsOpacityEffect>
#include "AssetItem.h"
#include "../parameter/Analysis.h"
#include "../../common/storage/asset.h"

using namespace Picto;

//! [0]
class ExperimentItem : public AssetItem
{
	Q_OBJECT
public:
	ExperimentItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	virtual ~ExperimentItem(){};

protected:
	virtual void setRect(QRectF rect);
	virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	void currentAnalysisChanged(QSharedPointer<Analysis> currAnalysis);
private:
	void applyIconOpacity();
	//QGraphicsOpacityEffect* opacityEffect_;
};
//! [0]

#endif
