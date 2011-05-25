#ifndef DIAGRAMSCENEITEM_H
#define DIAGRAMSCENEITEM_H

#include "diagramscene.h"
#include "diagramitem.h"

//! [0]
class DiagramSceneItem : public DiagramScene, public DiagramItem
{
	Q_OBJECT
public:
	 DiagramSceneItem(QMenu *itemMenu, DiagramType diagramType, DiagramScene* parent);
	int testInt;
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
signals:
	void opened(DiagramScene* scene);
private:
	DiagramScene* parent_;

	//virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
};
//! [0]

#endif
