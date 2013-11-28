#ifndef ARROWSOURCEITEM_H
#define ARROWSOURCEITEM_H

#include "ArrowPortItem.h"

/*! \brief An ArrowPortItem to which Arrow tails attach.
 *	\details This is a base class for result bars that hang of the bottom of each StateMachineElement diagram 
 *	as well as the StartBar on the left side of the canvas.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class ArrowSourceItem : public ArrowPortItem
{
	Q_OBJECT
public:
	ArrowSourceItem(QString name,QSharedPointer<EditorState> editorState, QGraphicsItem *parent, QSharedPointer<Asset> asset);
	virtual ~ArrowSourceItem(){};
	virtual QPointF getContactPoint();
protected:
	virtual void setRect(QRectF rect);
	virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
};


#endif
