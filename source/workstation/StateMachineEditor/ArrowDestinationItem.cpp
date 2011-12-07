#include "ArrowDestinationItem.h"
#include "ArrowSourceItem.h"
#include "Arrow.h"
#include <QGraphicsScene>
#include "../../common/memleakdetect.h"
ArrowDestinationItem::ArrowDestinationItem(QSharedPointer<EditorState> editorState,QGraphicsItem *parent, QGraphicsScene *scene) :
ArrowPortItem("",editorState,parent,scene)
{
	//QLinearGradient grad(QPointF(0,-20),QPointF(0,20));
	//grad.setColorAt(0,QColor(Qt::green));
	//grad.setColorAt(1,QColor(Qt::darkGreen));
	//QBrush brush(grad);
	//setBrush(brush);
	//setSvgIcon(QPixmap(":/icons/start.png"));
}

QPointF ArrowDestinationItem::getContactPoint()
{
	return getRect().center()-QPointF(getRect().width()/2.0,0);
}

void ArrowDestinationItem::setRect(QRectF rect)
{
	ArrowPortItem::setRect(rect);
	QLinearGradient grad(getRect().topLeft(),getRect().bottomLeft());
	grad.setColorAt(0,QColor(0,210,0));
	grad.setColorAt(1,QColor(0,130,0));
	QBrush brush(grad);
	setBrush(brush);
}