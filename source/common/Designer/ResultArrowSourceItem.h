#ifndef ResultArrowSourceItem_H
#define ResultArrowSourceItem_H

#include "ArrowSourceItem.h"
#include "ScriptItemManager.h"

/*! \brief An ArrowSourceItem representing a result that hangs off of a diagram representing the StateMachineElement which contains it.
 *	\details DiagramItems representing StateMachineElements show all of their child results hanging off of the bottom.  This item 
 *	represents one of these results.  It is an ArrowSourceItem and contains a ScriptItemManager to display the EntryScript of the 
 *	Result that it represents.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class ResultArrowSourceItem : public ArrowSourceItem
{
	Q_OBJECT
public:
	ResultArrowSourceItem(QString name,QSharedPointer<EditorState> editorState, QGraphicsItem *parent, QSharedPointer<Asset> windowAsset);
	virtual ~ResultArrowSourceItem(){};
protected:
	virtual void setRect(QRectF rect);
private:
	QSharedPointer<ScriptItemManager> scriptItemManager_;
};


#endif
