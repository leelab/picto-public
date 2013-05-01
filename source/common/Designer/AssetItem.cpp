#include "AssetItem.h"
#include "../../common/property/property.h"
#include "../../common/statemachine/uienabled.h"
#include <QPen>
#include <QBrush>
#include "../../common/memleakdetect.h"
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

	//QPen pen;
	//pen.setWidth(1);
	//pen.setColor(QColor(Qt::blue));
	//setPen(pen);

	//QLinearGradient grad(QPointF(0,-20),QPointF(0,20));
	//grad.setColorAt(0,QColor(240,240,0));
	//grad.setColorAt(1,QColor(180,180,0));
	//QBrush brush(grad);
	//setBrush(brush);
	//setRect(QRectF(QPointF(-19,-19),QPointF(19,19)));
	//setSvgIcon(QPixmap(":/icons/filenew.png"));


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

void AssetItem::assetEdited()
{
	QString name = asset_->getName();
	if(name == "")
		name = "NO NAME";
	setName(name,QPointF(-100,-100));
	QString type = asset_->assetType();
	if(type == "")
		type = "UNDEFINED TYPE";
	setType(type);

	//If a search is already going on, trigger searchRequested now in case the edit affects the search
	foreach(SearchRequest searchRequest,editorState_->getSearchRequests())
	{
		searchRequested(searchRequest);
	}
}

void AssetItem::positionChanged(QPoint pos)
{
	if(asset_.isNull())
		return;
	asset_.staticCast<UIEnabled>()->setPos(pos);
	if(posInitialized_)
		posChanged_ = true;
	posInitialized_ = true;
}

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

void AssetItem::hoverEnterEvent ( QGraphicsSceneHoverEvent * )
{
	if((editorState_->getEditMode() != EditorState::DrawLine) && (editorState_->getEditMode() != EditorState::PlaceItem))
	{
		editorState_->setEditMode(EditorState::MoveItem);
		//setCursor(Qt::SizeAllCursor);
	}
}

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

void AssetItem::setSvgIcon(QString svgFile)
{
	if(svgFile.isEmpty())
		return;
	svgFileName_ = svgFile;
	//Update everything now that we have a new file
	setRect(getRect());
}

void AssetItem::keepPixmapAspectRatio(bool keep)
{
	keepAspectRatio_ = keep;
}

void AssetItem::setRect(QRectF rect)
{
	DiagramItem::setRect(rect);
	QLinearGradient grad(QPointF(0,-20),QPointF(0,20));
	float stopPoint = (getIconRect().top()-getRect().top())/getRect().height();
	grad.setColorAt(0,QColor("darkslategrey"));
	grad.setColorAt(stopPoint,stopPoint<1?"lightslategray":"darkslategrey");
	//grad.setColorAt(1,QColor("red"));
	QBrush brush(grad);
	setBrush(brush);
	setSvgIcon(getSvgItem());
}

QGraphicsSvgItem* AssetItem::getSvgItem()
{
	if(svgFileName_.isEmpty())
		return NULL;
	return new QGraphicsSvgItem(svgFileName_,this);
}

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
		highlightNameChars(searchRequest.getGroupTypeIndex(),searchRequest.query,searchRequest.caseSensitive);
}