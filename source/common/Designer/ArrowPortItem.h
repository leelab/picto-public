#ifndef ARROWPORTITEM_H
#define ARROWPORTITEM_H

#include <QSharedPointer>
#include "diagramitem.h"
#include "../storage/asset.h"

//! [0]
class ArrowPortItem : public DiagramItem
{
	Q_OBJECT
public:
	ArrowPortItem(QString name,QSharedPointer<EditorState> editorState,QGraphicsItem *parent,QSharedPointer<Asset> asset);
	virtual ~ArrowPortItem();
 	void addArrow(Arrow *arrow);
	void removeArrow(Arrow *arrow);
    void removeArrows();
	virtual QPointF getContactPoint() = 0;
	QSharedPointer<Asset> getAsset(){return asset_;};
protected:
	virtual void updateDependantGraphics();
	virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
	virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
	QSharedPointer<Asset> asset_;
private:
	QList<Arrow *> arrows;
};
//! [0]

#endif
