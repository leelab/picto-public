#ifndef ARROWDESTIONATIONITEM_H
#define ARROWDESTIONATIONITEM_H

#include <QMap>
#include <QVector>
#include <QColor>
#include "ArrowPortItem.h"
#include "ScriptItem.h"
#include "ScriptItemManager.h"

/*! \brief An ArrowPortItem to which Arrow heads attach.
 *	\details This is the start bar hanging off of the side of each StateMachineElement diagram in
 *	in the designer canvas.  Beyond being an ArrowPortItem that draws a green bar and 
 *	defines an arrow head contact point, it also includes a ScriptItemManager that defines
 *	the position of ScriptItem boxes along its length.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class ArrowDestinationItem : public ArrowPortItem
{
	Q_OBJECT
public:
	ArrowDestinationItem(QSharedPointer<EditorState> editorState, QGraphicsItem *parent, QSharedPointer<Asset> asset);
	virtual ~ArrowDestinationItem(){};
	virtual QPointF getContactPoint();
protected:
	virtual void setRect(QRectF rect);
private:
	QSharedPointer<ScriptItemManager> scriptItemManager_;
};


#endif
