#ifndef ARROWSOURCEITEM_H
#define ARROWSOURCEITEM_H

#include "ArrowPortItem.h"

//! [0]
class ArrowSourceItem : public ArrowPortItem
{
	Q_OBJECT
public:
	ArrowSourceItem(QString name,QSharedPointer<EditorState> editorState, QMenu* contextMenu, QGraphicsItem *parent, QSharedPointer<Asset> asset);
	virtual ~ArrowSourceItem(){};
	virtual QPointF getContactPoint();
protected:
	virtual void setRect(QRectF rect);
};
//! [0]

#endif
