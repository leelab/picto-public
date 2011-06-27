#include "ArrowDestinationItem.h"
#include "ArrowSourceItem.h"
#include "Arrow.h"
#include <QGraphicsScene>
ArrowDestinationItem::ArrowDestinationItem(QString name,QSharedPointer<EditorState> editorState,QGraphicsItem *parent, QGraphicsScene *scene) :
ArrowPortItem(name,editorState,parent,scene)
{
}