#include "ArrowSourceItem.h"
#include "ArrowDestinationItem.h"
#include "Arrow.h"
#include <QGraphicsScene>
#include "../../common/memleakdetect.h"
ArrowSourceItem::ArrowSourceItem(QString name,QSharedPointer<EditorState> editorState, QGraphicsItem *parent, QSharedPointer<Asset> asset) :
ArrowPortItem(name,editorState, parent,asset)
{
	QLinearGradient grad(QPointF(0,-20),QPointF(0,20));
	grad.setColorAt(0,QColor(Qt::red));
	grad.setColorAt(1,QColor(Qt::darkRed));
	QBrush brush(grad);
	setBrush(brush);
	//setSvgIcon(QPixmap(":/icons/result.png"));
}

QPointF ArrowSourceItem::getContactPoint()
{
	return getRect().center()+QPointF(getRect().width()/2.0,0);
}

void ArrowSourceItem::setRect(QRectF rect)
{
	ArrowPortItem::setRect(rect);
	QLinearGradient grad(getRect().topLeft(),getRect().bottomLeft());
	grad.setColorAt(0,QColor(210,0,0));
	grad.setColorAt(1,QColor(130,0,0));
	QBrush brush(grad);
	setBrush(brush);
}