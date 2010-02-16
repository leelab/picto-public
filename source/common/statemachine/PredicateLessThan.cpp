#include <QPainter>
#include <QPoint>

#include "PredicateLessThan.h"

namespace Picto
{
PredicateLessThan::PredicateLessThan()
: Predicate()
{
	predicateString_= "<";
	name_ = "Less than";

	//draw the icon
	const QPoint points[] = { QPoint(75,30),
							   QPoint(25,50),
							   QPoint(75,70)};

	QPainter p(&icon_);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(QColor(0,0,0,255));

	QPen pen(QColor(0,0,0,255));
	pen.setWidth(4);
	pen.setJoinStyle(Qt::MiterJoin);
	p.setPen(pen);

	p.drawPolyline(points,3);
	p.end();


}

bool PredicateLessThan::evaluate(double LHS, double RHS)
{
	return (LHS < RHS);
}

} //namespacePicto