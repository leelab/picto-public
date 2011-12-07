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
svgItem_(NULL)
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
	assetEdited();
	connect(asset_.data(),SIGNAL(edited()),this,SLOT(assetEdited()));
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

void AssetItem::setSvgIcon(QGraphicsSvgItem* svgIcon)
{
	if(!svgIcon)
		return;
	if(svgItem_ && (svgItem_ != svgIcon))
	{
		svgItem_->setParent(NULL);
		delete svgItem_;
	}
	svgItem_ = svgIcon;
	svgItem_->setParentItem(this);
	QRectF boundRect = svgItem_->boundingRect();
	//Find the scale factor (always scale according to the smaller of width or height scale factors.
	float widthScale = getIconRect().width()/boundRect.width();
	float heightScale = getIconRect().height()/boundRect.height();
	float smallerScale = (widthScale > heightScale)?heightScale:widthScale;
	svgItem_->setTransform(svgItem_->transform().scale(smallerScale,smallerScale));
	//svgItem_->setSvgIcon(pixmap.scaled(getIconRect().width(),getIconRect().height(),keepAspectRatio_?Qt::KeepAspectRatio:Qt::IgnoreAspectRatio));
	svgItem_->setPos(getIconRect().center()-QPointF(smallerScale*boundRect.width()/2.0,smallerScale*boundRect.height()/2.0));
	svgItem_->setZValue(0);
}

void AssetItem::setSvgIcon(QString svgFile)
{
	if(svgFile.isEmpty())
		return;
	QGraphicsSvgItem* newItem = new QGraphicsSvgItem(svgFile,this);
	setSvgIcon(newItem);
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
	setSvgIcon(svgItem_);
}