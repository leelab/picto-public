#include "StartBarItem.h"
#include "ArrowDestinationItem.h"
#include "Arrow.h"
#include <QGraphicsScene>
#include "../../common/memleakdetect.h"
StartBarItem::StartBarItem(QString name,QSharedPointer<EditorState> editorState, QMenu* contextMenu, QGraphicsItem *parent, QSharedPointer<Asset> windowAsset) :
ArrowSourceItem(name,editorState,contextMenu,parent,windowAsset)
{
	setPos(QPointF(0,0));
	setWidth(20);
	setHeight(4000);
	//setSvgIcon(QPixmap(":/icons/result.png"));
}

QPointF StartBarItem::getContactPoint()
{
	return getRect().center()+QPointF(getRect().width()/2.0,0);
}

void StartBarItem::setRect(QRectF rect)
{
	ArrowSourceItem::setRect(rect);
	QLinearGradient grad(getRect().topLeft(),getRect().bottomLeft());
	grad.setColorAt(0,QColor(0,210,0));
	grad.setColorAt(1,QColor(0,130,0));
	QBrush brush(grad);
	setBrush(brush);
}