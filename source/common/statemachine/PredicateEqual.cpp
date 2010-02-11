#include <QPainter>
#include <QPoint>

#include "PredicateEqual.h"

namespace Picto
{
PredicateEqual::PredicateEqual()
: Predicate()
{
	predicateString_= "==";

	//draw the icon

	QPainter p(&icon_);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(QColor(0,0,0,255));

	QPen pen(QColor(0,0,0,255));
	pen.setWidth(4);
	pen.setJoinStyle(Qt::MiterJoin);
	p.setPen(pen);

	p.drawLine(15,60,40,60);
	p.drawLine(15,40,40,40);
	p.drawLine(60,60,85,60);
	p.drawLine(60,40,85,40);
	p.end();


}

bool PredicateEqual::evaluate(double LHS, double RHS)
{
	return (LHS == RHS);
}

} //namespacePicto