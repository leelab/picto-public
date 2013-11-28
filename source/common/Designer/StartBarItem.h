#ifndef STARTBARITEM_H
#define STARTBARITEM_H

#include "ArrowSourceItem.h"
#include "ScriptItemManager.h"

/*! \brief This is a green bar that appears at the left side of the canvas to represent the starting position for
 *	control flow within a StateMachine.
 *	\details The StartBarItem includes a ScriptItemManager that handles ScriptItem boxes for each of the StateMachine
 *	element's Scripts.  The StartBar also always makes sure to let its ScriptItemManager know about scrolling changes so
 *	that all scripts will appear on screen regardless of the user's scroll position.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
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


#endif
