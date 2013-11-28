#ifndef WIREABLEITEM_H
#define WIREABLEITEM_H

#include <QSharedPointer>
#include <QMenu>
#include "ExperimentItem.h"
#include "../../common/storage/asset.h"

using namespace Picto;

class Arrow;

/*! \brief An ExperimentItem that represents a StateMachineElement.  It is a "Wireable Item" in that it takes part in control
 *	flow and can therefore have transitions wired to it.
 *	\details WireableItem objects can include an attached start bar destination for arrows and functionality for adding result arrow sources
 *	\sa ResultArrowSourceItem, ArrowDestinationItem
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class WireableItem : public ExperimentItem
{
	Q_OBJECT
public:
	WireableItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset);
	virtual ~WireableItem();
	void addResultArrowSource(QSharedPointer<Asset> sourceAsset);
	void enableArrowDest();
	QList<DiagramItem*> getArrowSources();
	DiagramItem* getArrowDest();
protected:
	virtual void updateDependantGraphics(){};
	virtual void setRect(QRectF rect);
private:
	void updateArrowPortDimensions();
	QList<DiagramItem*> arrowSources_;
	DiagramItem* arrowDest_;
	float maxArrowSourceWidth_;

};


#endif
