#ifndef DIAGRAMITEMFACTORY_H
#define DIAGRAMITEMFACTORY_H

#include "diagramitem.h"
#include "../../common/storage/asset.h"
using namespace Picto;

//! [0]
class DiagramItemFactory
{
public:
	DiagramItemFactory(QMenu *contextMenu, QGraphicsScene *scene);
	DiagramItem* create(QSharedPointer<Asset> asset);
private:
	QMenu* contextMenu_;
	QGraphicsScene* scene_;
};
//! [0]

#endif
