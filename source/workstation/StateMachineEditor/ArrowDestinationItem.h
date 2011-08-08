#ifndef ARROWDESTIONATIONITEM_H
#define ARROWDESTIONATIONITEM_H

#include "ArrowPortItem.h"

//! [0]
class ArrowDestinationItem : public ArrowPortItem
{
	Q_OBJECT
public:
	ArrowDestinationItem(QSharedPointer<EditorState> editorState,QGraphicsItem *parent, QGraphicsScene *scene);
	virtual QPointF getContactPoint();
protected:
	virtual void setRect(QRectF rect);
};
//! [0]

#endif
