#ifndef ARROWPORTITEM_H
#define ARROWPORTITEM_H

#include "diagramitem.h"

//! [0]
class ArrowPortItem : public DiagramItem
{
	Q_OBJECT
public:
	ArrowPortItem(QString name,QGraphicsItem *parent, QGraphicsScene *scene);
 	void addArrow(Arrow *arrow);
	void removeArrow(Arrow *arrow);
    void removeArrows();
	virtual void removeDependantGraphics();
protected:
	virtual void updateDependantGraphics();
private:
	QList<Arrow *> arrows;
};
//! [0]

#endif
