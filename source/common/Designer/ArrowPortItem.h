#ifndef ARROWPORTITEM_H
#define ARROWPORTITEM_H

#include <QSharedPointer>
#include "diagramitem.h"
#include "../storage/asset.h"

/*! \brief A box that acts as a port for the tail or head of a transition arrow.
 *	\details This ArrowPortItem takes care of things like telling the arrow connected to it if it
 *	needs to redraw itself with new endpoints and deleting an attached arrow if this ArrowPortItem
 *	is deleted.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class ArrowPortItem : public DiagramItem
{
	Q_OBJECT
public:
	ArrowPortItem(QString name,QSharedPointer<EditorState> editorState,QGraphicsItem *parent,QSharedPointer<Asset> asset);
	virtual ~ArrowPortItem();
 	void addArrow(Arrow *arrow);
	void removeArrow(Arrow *arrow);
    void removeArrows();
	/*! \brief The idea of this function was to define a point where the arrow should contact it.
 	 *	in practice, this function is not used.
	 */
	virtual QPointF getContactPoint() = 0;
	/*! \brief Returns the Asset associated with this ArrowPortItem. */ 
	QSharedPointer<Asset> getAsset(){return asset_;};
protected:
	virtual void updateDependantGraphics();
	virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	QSharedPointer<Asset> asset_;
private:
	QList<Arrow *> arrows;
};


#endif
