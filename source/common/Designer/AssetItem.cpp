#include "AssetItem.h"
#include "../../common/property/property.h"
#include "../../common/statemachine/uienabled.h"
#include <QPen>
#include <QBrush>
#include "../../common/memleakdetect.h"
/*! \brief Constructs a new AssetItem for the input Asset.  Other parameters are passed to DiagramItem.
 */
AssetItem::AssetItem(QSharedPointer<EditorState> editorState, QMenu *contextMenu, QSharedPointer<Asset> asset) :
DiagramItem(editorState,contextMenu),
asset_(asset),
posChanged_(false),
posInitialized_(false),
keepAspectRatio_(true),
svgFileName_(""),
lastSvgIcon_(NULL)
{
	QPen invisiblePen;
	invisiblePen.setWidth(0);
	invisiblePen.setColor(QColor(0,0,0,0));
	setPen(invisiblePen);

	//Set up search functionality
	//Set highlight colors for different groups of search
	setHighlightColor(SearchRequest::getGroupTypeIndex(SearchRequest::ACTIVE_ANALYSES,SearchRequest::SCRIPT),QColor(255,255,0,180));
	setHighlightColor(SearchRequest::getGroupTypeIndex(SearchRequest::EXPERIMENT,SearchRequest::SCRIPT),QColor(0,0,255,220));
	setHighlightColor(SearchRequest::getGroupTypeIndex(SearchRequest::ACTIVE_ANALYSES,SearchRequest::STRING),QColor(255,0,0,180));
	setHighlightColor(SearchRequest::getGroupTypeIndex(SearchRequest::EXPERIMENT,SearchRequest::STRING),QColor(255,0,0,180));

	//Connect search signal to this object
	connect(editorState.data(),SIGNAL(searchRequested(SearchRequest)),this,SLOT(searchRequested(SearchRequest)));

	//Initialize data by triggering asset edited
	assetEdited();
	connect(asset_.data(),SIGNAL(edited()),this,SLOT(assetEdited()));
	if(asset_->inherits("Picto::DataStore"))
		connect(asset_.staticCast<DataStore>().data(),SIGNAL(associateChildEdited()),this,SLOT(assetEdited()));
	//setCursor(Qt::SizeAllCursor);
}

AssetItem::~AssetItem()
{
}
/*! \brief Called when the underlying Asset is edited to update the displayed information accordingly.
 *	\details The AssetItem shows the Asset's name, type, notes, and highlights the search string in
 *	its name.  Most of these aspects can be changed during design development, so this needs to take 
 *	care of updating the UI accordingly.
 */
void AssetItem::assetEdited()
{
	QString name = asset_->getName();
	if(name == "")
		name = "NO NAME";
	setName(name,QPointF(-100,-100));
	QString type = asset_->friendlyTypeName();
	if(type == "")
		type = "UNDEFINED TYPE";
	setType(type);
	//Set the tooltip to the operator's notes
	Q_ASSERT(asset_->inherits("Picto::UIEnabled"));
	setToolTip(asset_.staticCast<UIEnabled>()->getNotes());

	//If a search is already going on, trigger searchRequested now in case the edit affects the search
	foreach(SearchRequest searchRequest,editorState_->getSearchRequests())
	{
		searchRequested(searchRequest);
	}
}

/*! \copydoc DiagramItem::positionChanged()
 *	\details In the case of the AssetItem, this updates the saved UI position of the Asset when it is moved.
 */
void AssetItem::positionChanged(QPoint pos)
{
	if(asset_.isNull())
		return;
	asset_.staticCast<UIEnabled>()->setPos(pos);
	if(posInitialized_)
		posChanged_ = true;
	posInitialized_ = true;
}

/*! \brief When the mouse button is released, if the position changed we need to make
 *	the move undoable.  This handles that.
 */
void AssetItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if(posChanged_)
	{
		if(!getEditorState().isNull())
			getEditorState()->setLastActionUndoable();
		posChanged_ = false;
	}
	QGraphicsItem::mouseReleaseEvent(event);
}

/*! \brief Changes the edit mode to the EditorState::EditMode to MoveItem when the mouse enters this AssetItem's bounds.
*/
void AssetItem::hoverEnterEvent ( QGraphicsSceneHoverEvent * )
{
	if((editorState_->getEditMode() != EditorState::DrawLine) && (editorState_->getEditMode() != EditorState::PlaceItem))
	{
		editorState_->setEditMode(EditorState::MoveItem);
		//setCursor(Qt::SizeAllCursor);
	}
}
/*! \brief Sets the AssetItem's graphic to the input SVG icon.*/
void AssetItem::setSvgIcon(QGraphicsSvgItem* svgIcon)
{
	if(!svgIcon)
		return;
	if(lastSvgIcon_ && (lastSvgIcon_ != svgIcon))
	{
		lastSvgIcon_->setParent(NULL);
		delete lastSvgIcon_;
	}
	lastSvgIcon_ = svgIcon;
	svgIcon->setParentItem(this);
	QRectF boundRect = svgIcon->boundingRect();
	//Find the scale factor (always scale according to the smaller of width or height scale factors.
	float widthScale = getIconRect().width()/boundRect.width();
	float heightScale = getIconRect().height()/boundRect.height();
	float smallerScale = (widthScale > heightScale)?heightScale:widthScale;
	svgIcon->setTransform(svgIcon->transform().scale(smallerScale,smallerScale));
	//svgItem_->setSvgIcon(pixmap.scaled(getIconRect().width(),getIconRect().height(),keepAspectRatio_?Qt::KeepAspectRatio:Qt::IgnoreAspectRatio));
	svgIcon->setPos(getIconRect().center()-QPointF(smallerScale*boundRect.width()/2.0,smallerScale*boundRect.height()/2.0));
	svgIcon->setZValue(0);
}

/*! \brief Sets the AssetItem's graphic to the SVG icon in the input file.*/
void AssetItem::setSvgIcon(QString svgFile)
{
	if(svgFile.isEmpty())
		return;
	svgFileName_ = svgFile;
	//setRect causes setSvgIcon(QGraphicsSvgItem*) to get called with the SVG icon in the file.
	setRect(getRect());
}

/*! \brief This does not appear to be used anymore.  We should probably get rid of it.
*/
void AssetItem::keepPixmapAspectRatio(bool keep)
{
	keepAspectRatio_ = keep;
}

/*! \brief Whenever the size of this AssetItem is changed, its SVG icon needs to be updated accordingly.  This takes care of that.
 */
void AssetItem::setRect(QRectF rect)
{
	DiagramItem::setRect(rect);
	//QLinearGradient grad(QPointF(0,-20),QPointF(0,20));
	//float stopPoint = (getIconRect().top()-getRect().top())/getRect().height();
	//grad.setColorAt(0,QColor("darkslategrey"));
	//grad.setColorAt(stopPoint,stopPoint<1?"lightslategray":"darkslategrey");
	////grad.setColorAt(1,QColor("red"));
	//QBrush brush(grad);
	//setBrush(brush);
	setSvgIcon(getSvgItem());
}

/*! \brief Constructs and returns a QGraphicsSvgItem from the SVG Icon file set for this AssetItem in 
 *	setSvgIcon(QString).
 *	\details Returns a NULL pointer if no svgFileName_ has been set.
 */
QGraphicsSvgItem* AssetItem::getSvgItem()
{
	if(svgFileName_.isEmpty())
		return NULL;
	return new QGraphicsSvgItem(svgFileName_,this);
}

/*! \brief Called when a search has been requested to highlight the search string in the AssetItem's name and/or outline the
 *	AssetItem if the searchRequest is found at a level underneath this asset.
 */
void AssetItem::searchRequested(SearchRequest searchRequest)
{
	//if(searchRequest.group == SearchRequest::ACTIVE_ANALYSES)
	//	return;
	QSharedPointer<ScriptableContainer> scriptableContainer;
	if(asset_->inherits("Picto::ScriptableContainer"))
		scriptableContainer = asset_.staticCast<ScriptableContainer>();
	
	//Determine if search outline should be turned on or off and do it
	bool needsOutline = false;
	if(scriptableContainer)
	{
			//If the search is enabled
		if	( searchRequest.enabled)
		{
			//If this is a MachineContainer
			if(scriptableContainer->inherits("Picto::MachineContainer"))
			{
				//Search the MachineContainer's kids for the request
				if(scriptableContainer->searchChildrenRecursivelyForQuery(searchRequest))
				{
					needsOutline = true;
				}
			}
			else if(!scriptableContainer->inherits("Picto::Result"))	
			{	//This is not a MachineContainer or a result
				//Search the ScriptableContainer itself for the query
				if(scriptableContainer->searchRecursivelyForQuery(searchRequest))
				{
					needsOutline = true;
				}
			}
		}
		//Create the outline
		if(needsOutline)
			enableOutline(searchRequest.getGroupTypeIndex(),true);
		else
			enableOutline(searchRequest.getGroupTypeIndex(),false);
	}

	//Handle name highlighting
	if(searchRequest.type == SearchRequest::STRING)
		highlightNameChars(SearchRequest::getGroupTypeIndex(SearchRequest::EXPERIMENT,SearchRequest::STRING),searchRequest.query,searchRequest.caseSensitive);
}