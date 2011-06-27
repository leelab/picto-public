#include "ArrowSourceItem.h"
#include "ArrowDestinationItem.h"
#include "Arrow.h"
#include <QGraphicsScene>
ArrowSourceItem::ArrowSourceItem(QString name,QSharedPointer<EditorState> editorState,QGraphicsItem *parent, QGraphicsScene *scene) :
ArrowPortItem(name,editorState,parent,scene)
{
}