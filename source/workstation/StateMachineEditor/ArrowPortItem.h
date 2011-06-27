#ifndef ARROWPORTITEM_H
#define ARROWPORTITEM_H

#include "diagramitem.h"

//! [0]
class ArrowPortItem : public DiagramItem
{
	Q_OBJECT
public:
	ArrowPortItem(QString name,QSharedPointer<EditorState> editorState,QGraphicsItem *parent, QGraphicsScene *scene);
	virtual ~ArrowPortItem();
 	void addArrow(Arrow *arrow);
	void removeArrow(Arrow *arrow);
    void removeArrows();
protected:
	virtual void updateDependantGraphics();
private:
	QList<Arrow *> arrows;
};
//! [0]

#endif
