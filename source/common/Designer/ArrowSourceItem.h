#ifndef ARROWSOURCEITEM_H
#define ARROWSOURCEITEM_H

#include "ArrowPortItem.h"

//! [0]
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
//! [0]

#endif
