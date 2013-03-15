#ifndef ARROWDESTIONATIONITEM_H
#define ARROWDESTIONATIONITEM_H

#include <QMap>
#include <QVector>
#include <QColor>
#include "ArrowPortItem.h"
#include "ScriptItem.h"
#include "ScriptItemManager.h"

//! [0]
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
//! [0]

#endif
