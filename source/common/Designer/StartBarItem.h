#ifndef STARTBARITEM_H
#define STARTBARITEM_H

#include "ArrowSourceItem.h"
#include "ScriptItemManager.h"

//! [0]
class StartBarItem : public ArrowSourceItem
{
	Q_OBJECT
public:
	StartBarItem(QString name,QSharedPointer<EditorState> editorState, QGraphicsItem *parent, QSharedPointer<Asset> windowAsset);
	virtual ~StartBarItem(){};
	virtual QPointF getContactPoint();
	virtual void paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
protected:
	virtual void setRect(QRectF rect);
private:
	void updateForViewportChanges();
	QRectF scriptRect_;
	QSharedPointer<ScriptItemManager> scriptItemManager_;
};
//! [0]

#endif
