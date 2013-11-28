#ifndef ExperimentItem_H
#define ExperimentItem_H

#include <QSharedPointer>
#include <QGraphicsOpacityEffect>
#include "AssetItem.h"
#include "../parameter/Analysis.h"
#include "../../common/storage/asset.h"

using namespace Picto;

/*! \brief An AssetItem used to represent Experimental (ie. Non-Analysis) elements.
 *	\details This is pretty much just an AssetItem that is colored greyish to represent that
 *	it is part of the experiment and not the analysis.  It also has functionality for disabling
 *	certain capabilities when the Designer is in "Analysis design" mode.
 *	\sa EditorState::setCurrentAnalysis()
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
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


#endif
