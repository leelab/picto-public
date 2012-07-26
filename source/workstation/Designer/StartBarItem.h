#ifndef STARTBARITEM_H
#define STARTBARITEM_H

#include "ArrowSourceItem.h"

//! [0]
class StartBarItem : public ArrowSourceItem
{
	Q_OBJECT
public:
	StartBarItem(QString name,QSharedPointer<EditorState> editorState,QGraphicsItem *parent, QGraphicsScene *scene);
	virtual ~StartBarItem(){};
	virtual QPointF getContactPoint();
protected:
	virtual void setRect(QRectF rect);
};
//! [0]

#endif
