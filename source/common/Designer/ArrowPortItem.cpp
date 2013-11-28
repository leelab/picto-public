#include "ArrowPortItem.h"
#include "ArrowDestinationItem.h"
#include "Arrow.h"
#include <QGraphicsScene>
#include "../../common/memleakdetect.h"
/*! \brief Constructs a new ArrowPortItem associated with the input Asset.  Other parameters are passed directly to the DiagramItem constructor.
*/
ArrowPortItem::ArrowPortItem(QString name,QSharedPointer<EditorState> editorState, QGraphicsItem *parent,QSharedPointer<Asset> asset) :
DiagramItem(editorState,NULL,name,parent)
{
	asset_ = asset;
	QPen invisiblePen;
	invisiblePen.setWidth(0);
	invisiblePen.setColor(QColor(0,0,0,0));
	setPen(invisiblePen);
	//setFlag(QGraphicsItem::ItemIsSelectable,false);
	setFlag(QGraphicsItem::ItemIsMovable,false);
	//setCursor(Qt::ArrowCursor);
}

/*! \brief When an ArrowPortItem is deleted, all Arrows attached to it are deleted as well.*/
ArrowPortItem::~ArrowPortItem()
{
	removeArrows();
}

/*! \brief Attached a new arrow to this ArrowPortItem.
*/
void ArrowPortItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}

/*! \brief Removes an Arrow from this ArrowPortItem without deleting it.
*/
void ArrowPortItem::removeArrow(Arrow *arrow)
{
    int index = arrows.indexOf(arrow);

    if (index != -1)
        arrows.removeAt(index);
}

/*! \brief Deletes all arrows that were attached to this ArrowPortItem and removes them from
 *	all other ArrowPortItems to which they were attached.
 *	\note This function doesn't clear the arrows list after deleting all the Arrow objects.  This isn't a problem
 *	since it is only ever called from the destructor anyway, but for the sake of future proofing
 *	it probably should do that.
 */
void ArrowPortItem::removeArrows()
{
    foreach (Arrow *arrow, arrows) {
        static_cast<ArrowPortItem*>(arrow->startItem())->removeArrow(arrow);
        static_cast<ArrowDestinationItem*>(arrow->endItem())->removeArrow(arrow);
        delete arrow;
    }
}

/*! \brief Tells all attached Arrow objects to redraw themselves when this ArrowPortItem moves.
*/
void ArrowPortItem::updateDependantGraphics()
{
    foreach (Arrow *arrow, arrows) 
	{
        arrow->updatePosition();
    }
}

/*! \brief Sets the EditorState::EditMode to Select mode when the mouse is over this ArrowPortItem.
*/
void ArrowPortItem::hoverEnterEvent ( QGraphicsSceneHoverEvent *)
{
	if((editorState_->getEditMode() != EditorState::DrawLine) && (editorState_->getEditMode() != EditorState::PlaceItem))
	{
		editorState_->setEditMode(EditorState::Select);
		//setCursor(Qt::ArrowCursor);
	}
}

/*! \brief Sets the EditorState::EditMode to a logical mode when leaving this ArrowPortItem.
*/
void ArrowPortItem::hoverLeaveEvent ( QGraphicsSceneHoverEvent * )
{
	if((editorState_->getEditMode() != EditorState::DrawLine) && (editorState_->getEditMode() != EditorState::PlaceItem))
	{
		//Since the mouse is actually already on top of the parent of this arrow port, we need to set the edit mode back
		//to something logical for our parent.  MoveItem if there is no analysis, and select if there is.
		if(editorState_->getCurrentAnalysis().isNull())
			editorState_->setEditMode(EditorState::MoveItem);
		else
			editorState_->setEditMode(EditorState::Select);
	}
}