#ifndef DIAGRAMITEMFACTORY_H
#define DIAGRAMITEMFACTORY_H

#include "diagramitem.h"
#include "../../common/storage/asset.h"
using namespace Picto;

//! [0]
class DiagramItemFactory
{
public:
	DiagramItemFactory(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QGraphicsScene *scene);
	DiagramItem* create(QSharedPointer<Asset> asset);
private:
	QSharedPointer<EditorState> editorState_;
	QMenu* contextMenu_;
	QGraphicsScene* scene_;
};
//! [0]

#endif
