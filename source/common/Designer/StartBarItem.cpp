#include "StartBarItem.h"
#include "ArrowDestinationItem.h"
#include "Arrow.h"
#include <QGraphicsScene>
#include "../../common/memleakdetect.h"
StartBarItem::StartBarItem(QString name,QSharedPointer<EditorState> editorState,QGraphicsItem *parent, QGraphicsScene *scene) :
ArrowSourceItem(name,editorState,parent,scene)
{
	QLinearGradient grad(QPointF(0,-20),QPointF(0,20));
	grad.setColorAt(0,QColor(Qt::red));
	grad.setColorAt(1,QColor(Qt::darkRed));
	QBrush brush(grad);
	setBrush(brush);
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
	grad.setColorAt(0,QColor(210,0,0));
	grad.setColorAt(1,QColor(130,0,0));
	QBrush brush(grad);
	setBrush(brush);
}