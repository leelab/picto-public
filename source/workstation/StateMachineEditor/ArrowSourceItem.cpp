#include "ArrowSourceItem.h"
#include "ArrowDestinationItem.h"
#include "Arrow.h"
#include <QGraphicsScene>
ArrowSourceItem::ArrowSourceItem(QString name,QGraphicsItem *parent, QGraphicsScene *scene) :
ArrowPortItem(name,parent,scene)
{
}