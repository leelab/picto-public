#ifndef STARTBARITEM_H
#define STARTBARITEM_H

#include "ArrowSourceItem.h"

//! [0]
class StartBarItem : public ArrowSourceItem
{
	Q_OBJECT
public:
	StartBarItem(QString name,QSharedPointer<EditorState> editorState, QMenu* contextMenu, QGraphicsItem *parent, QSharedPointer<Asset> windowAsset);
	virtual ~StartBarItem(){};
	virtual QPointF getContactPoint();
protected:
	virtual void setRect(QRectF rect);
};
//! [0]

#endif
