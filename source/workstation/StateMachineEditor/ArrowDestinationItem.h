#ifndef ARROWDESTIONATIONITEM_H
#define ARROWDESTIONATIONITEM_H

#include "ArrowPortItem.h"

//! [0]
class ArrowDestinationItem : public ArrowPortItem
{
	Q_OBJECT
public:
	ArrowDestinationItem(QString name,QGraphicsItem *parent, QGraphicsScene *scene);
};
//! [0]

#endif
