#ifndef ARROWSOURCEITEM_H
#define ARROWSOURCEITEM_H

#include "ArrowPortItem.h"

//! [0]
class ArrowSourceItem : public ArrowPortItem
{
	Q_OBJECT
public:
	ArrowSourceItem(QString name,QSharedPointer<EditorState> editorState,QGraphicsItem *parent, QGraphicsScene *scene);
};
//! [0]

#endif
