#include <QtWidgets>

#include "diagramitem.h"
#include "arrow.h"
#include "../../common/memleakdetect.h"

/*! \brief Constructs a new DiagramItem with the input name and parent, that will interface with the input EditorState and display the input contextMenu when right clicked.
 *	\sa EditorState, QMenu
*/
DiagramItem::DiagramItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QString name,
             QGraphicsItem *parent) : 
QGraphicsPolygonItem(parent/*, scene*/)
{
	editorState_ = editorState;
    myContextMenu = contextMenu;
	nameText_ = NULL;
	stickInPlace_ = true;
	catchUpFrames_ = 0;
	searchHighlightIndex_ = 0;
	searchString_ = "";
	searchCaseSensitive_ = false;
	setName(name);
	connect(editorState_.data(),SIGNAL(editModeChanged(int)),this,SLOT(editModeChanged(int)));

	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	this->setAcceptHoverEvents(true);
}

DiagramItem::~DiagramItem()
{}

/*! \brief Sets the name of this diagram item and the position at which to place it.
 *	\details In practice, the pos input is not well supported in other parts of the 
 *	DiagramItem since we never actually used it.
 */
void DiagramItem::setName(QString name,QPointF pos)
{
	name_ = name;
	updateLabel();
	if(pos != QPointF())
		nameText_->setPos(pos);
}
/*! \brief Sets the type string for this DiagramItem that will be displayed on it.*/
void DiagramItem::setType(QString type)
{
	type_ = type;
	updateLabel();
}
/*! \brief Returns the name set in setName()*/
QString DiagramItem::getName()
{
	return name_;
}
/*! \brief Returns the type set in setType()*/
QString DiagramItem::getType()
{
	return type_;
}

/*! \brief Sets the width of the DiagramItem.*/
void DiagramItem::setWidth(float width)
{
	QRectF newRect = getRect();

	newRect.setWidth(width);
	setRect(newRect);
}

/*! \brief Returns the width of the DiagramItem.*/
float DiagramItem::getWidth()
{
	return rect_.width();
}
/*! \brief Sets the height of the DiagramItem.*/
void DiagramItem::setHeight(float height)
{
	QRectF newRect = getRect();
	newRect.setHeight(height);
	setRect(newRect);
}

/*! \brief Returns the height of the DiagramItem.*/
float DiagramItem::getHeight()
{
	return rect_.height();
}

/*! \brief Updates the name/type label on the DiagramItem based on the latest values set to setName() and setType()
*/
void DiagramItem::updateLabel()
{
	if(!nameText_)
	{
		nameText_ = new QGraphicsTextItem(this);
		nameText_->setDefaultTextColor(QColor(Qt::white));
		nameText_->setZValue(1000.0);
	}
	QString text = getName();

	//If there is currently a search string, highlight the appropriate characters in the name.
	if(!searchString_.isEmpty())
	{
		int foundPos = 0;
		QString colorStartTag = QString("<span style=\"background-color: %1\">")
			.arg(highlightColors_[searchHighlightIndex_].name());
		QString colorEndTag("</span>");
		while((foundPos = text.indexOf(searchString_,foundPos,searchCaseSensitive_?Qt::CaseSensitive:Qt::CaseInsensitive)) >= 0)
		{
			text = text.insert(foundPos+searchString_.length(),colorEndTag);
			text = text.insert(foundPos,colorStartTag);
			foundPos += searchString_.length()+colorStartTag.length()+colorEndTag.length();
		}
	}

	//Add the type info
	if((text != "") && (getType() != ""))
	{
		text.append("<br>");
	}
	if(getType() != "")
	{
		text.append("(").append(getType()).append(")");
	}		
	//Write the text string to the label
	nameText_->setHtml(text);

	//Find out how much room the label takes up
	//If the graphic needs to make room for this label, change its size accordingly
	QRectF textbound = nameText_->boundingRect();
	//I found that an empty name nameText_ still has a finite boundingRect for some reason.  This corrects that.
	if(text.isEmpty())
	{
		textbound.setWidth(0);
		textbound.setHeight(0);
	}
	QRectF polybound = polygon().boundingRect();
	bool needsToStretch = false;
	if(polybound.width() < textbound.width())
	{
		needsToStretch = true;
		polybound.setWidth(textbound.width());
	}
	if(polybound.height() < textbound.height())
	{
		needsToStretch = true;
		polybound.setHeight(textbound.height());
	}
	if(needsToStretch)
	{
		//Set the new size to the diagramItem so that the text fits.
		setRect(polybound);
	}
	nameText_->setPos(rect_.topLeft());
}

/*! \brief Creates a QPixmap based on this DiagramItem's polygon.
 *	\note This doesn't appear to be used
 */
QPixmap DiagramItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(polygon());

    return pixmap;
}

/*! \brief Called by the QGraphicsItem system to paint this DiagramItem onto the QGraphicsScene.
 *	\details Paints the DiagramItem shape and outline.  Text is handled by updateLabel().
 */
void DiagramItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	QGraphicsPolygonItem::paint(painter,option,widget);
	QRectF rect = getRect();
	QPainter::CompositionMode currCompMode = painter->compositionMode();
	int paintWidth = 2;
	Q_ASSERT(paintWidth > 0 && paintWidth%2 == 0);
	painter->translate(-paintWidth/2,-paintWidth/2);
	rect.setWidth(rect.width()+paintWidth);
	rect.setHeight(rect.height()+paintWidth);
	foreach(int highlightIndex,outlines_.keys())
	{
		Q_ASSERT(highlightColors_.contains(highlightIndex));
		painter->translate(paintWidth,paintWidth);
		rect.setWidth(rect.width()-(2*paintWidth));
		rect.setHeight(rect.height()-(2*paintWidth));
		painter->setPen(QPen(QBrush(highlightColors_[highlightIndex]),paintWidth));
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(rect);
	}
	painter->setCompositionMode(currCompMode);
}

/*! \brief Called by the QGraphicsItem system when the context menu should be shown.  Shows the menu
 *	ands sets this item as selected.
 */
void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
	if(myContextMenu)
	   myContextMenu->exec(event->screenPos());
}

/*! \brief Called by the QGraphicsScene system to handle selection/position updates when something about this item changes.  Handles the change.
 *	\details For position changes, we only accept significant changes so that double clicking will not
 *	end up changing diagram position.
 */
QVariant DiagramItem::itemChange(GraphicsItemChange change,
                     const QVariant &value)
{
	QVariant inputVal = value;
	//See if the change is a position change.
	if	(change == QGraphicsItem::ItemPositionChange)
	{
		//See how big the change is.  We don't accept small changes 
		//because they can get triggered by double clicking.
		QPointF translation = inputVal.toPointF() - pos();
		QRectF bounds = boundingRect();
		if	(
				stickInPlace_
			&&	isSelected()
			&&	(	(abs(translation.x()) > (bounds.width()/3))
				||	(abs(translation.y()) > (bounds.height()/3))
				)	
			)
			//This is a large change, stop sticking the item in place
			stickInPlace_ = false;

		//We only allow movement in MoveItem mode 
		if(editorState_->getEditMode() != EditorState::MoveItem)
			stickInPlace_ = true;

		if(!stickInPlace_)
		{
			if(catchUpFrames_ < 60)
			{
				//We haven't yet caught our position up from sticking in place.
				//This will move the item smoothly to the desired location after sticking.
				catchUpFrames_++;
				inputVal.setValue(pos()+((catchUpFrames_/60.0)*translation));
			}
		}
		if(!stickInPlace_ || !isSelected())
		{	//If we're not sticking, update graphics underneath this one and report
			//to child classes that the position change.
			//Note that we also do this if the item isn't selected.  This occurs when it
			//is being placed in the scene when it is first created.
			updateDependantGraphics();
			positionChanged(inputVal.toPoint());
		}
		else
		{	//The item is sticking right now.  Don't allow a position change.
			return pos();
		}
    }
	else if (change == QGraphicsItem::ItemSelectedChange)
	{	
		////Don't allow selection changes except in Select mode.
		//if(editorState_->getEditMode() != EditorState::Select)
		//	return !inputVal.toBool();
	}
	else if (change == QGraphicsItem::ItemSelectedHasChanged)
	{
		if(inputVal.toBool())
		{
			editorState_->setSelectedItem(this);
		}
		else
		{
			//When an item is deselected, it should be reinitialized to stick in place.
			stickInPlace_ = true;
			catchUpFrames_ = 0;

			//If this was the last selected item, set the selected item to NULL.  This
			//has the effect of switching the selected asset to the window asset.
			if(editorState_->getSelectedItem() == this)
				editorState_->setSelectedItem(NULL);
		}
	}


    return inputVal;
}
/*! \brief Called when the user double clicks on this DiagramItem.  Causes this item to be selected as the Window Asset.
*/
void DiagramItem::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
	if(/*editorState_->getEditMode() == EditorState::Select && */mouseEvent->button() == Qt::LeftButton)
	{
		if(!editorState_->getSelectedAsset().isNull())
		{
			editorState_->setWindowAsset(editorState_->getSelectedAsset());
		}
	}
}

/*! \brief Sets the dimensions of the DiagramItem to those of the input QRectF and updates contents accordingly.*/
void DiagramItem::setRect(QRectF rect)
{
	QRectF textbound;
	if(nameText_)
	{
		textbound = nameText_->boundingRect();
		if(rect.width() < textbound.width())
			rect.setWidth(textbound.width());
	}
	rect_ = rect;
	iconRect_ = QRectF(rect_.left(),rect_.top()+textbound.height(),rect_.width(),rect_.height()-textbound.height());
	setPolygon(QPolygonF(rect_));
	updateLabel();
}

/*! \brief Sets the highlight color for the input highlight index.
 *	\sa SearchRequest, SearchRequest::getGroupTypeIndex()
 */
void DiagramItem::setHighlightColor(int highlightIndex,QColor color)
{
	highlightColors_[highlightIndex] = color;
}

/*! \brief Enables/disables display of this DigramItem's outline for the input highlightIndex.
 *	\sa SearchRequest, SearchRequest::getGroupTypeIndex()
 */
void DiagramItem::enableOutline(int highlightIndex,bool enabled)
{
	//Loop through all enabled groups, and enable/disable outlines for each one according to the input
	unsigned int outlineIndex;
	unsigned short upperNybble = 0xF0 & highlightIndex; 
	for(unsigned short lowerNybble=0x01;lowerNybble<0x10;lowerNybble = lowerNybble<<1)
	{
		if(!(highlightIndex & lowerNybble))
			continue;
		outlineIndex = upperNybble | lowerNybble;
		if(!enabled)
		{
			if(outlines_.contains(outlineIndex))
			{
				outlines_.remove(outlineIndex);
				setRect(getRect());
				update();
			}
			return;
		}
		outlines_[outlineIndex] = true;
	}
	update(getRect());
}
/*! \brief Highlights the input searchString if found in this DiagramItem's name according to caseSensitive and with the color 
 *	set for highlightIndex by setHighlightColor()
 */
void DiagramItem::highlightNameChars(int highlightIndex, QString searchString, bool caseSensitive)
{
	searchString_ = searchString;
	searchHighlightIndex_ = highlightIndex;
	searchCaseSensitive_ = caseSensitive;
	updateLabel();
	update(getRect());
}
/*! \brief Returna QRectF with the dimensions of this DiagramItem
 */
QRectF DiagramItem::getRect()
{
	return rect_;
}

/*! \brief Doesn't do anything.
 *	\note We should probably remove DiagramItem::editModeChanged() because it appears to not do anything.
 */
void DiagramItem::editModeChanged(int)
{
	//if(mode == EditorState::Select || mode == EditorState::MoveItem)
	//{
	//	setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
	//}
	//else
	//{
	//	setAcceptedMouseButtons(Qt::RightButton);
	//}
}
