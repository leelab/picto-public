#include <QPainter>
#include <QPoint>

#include "PredicateGreaterThan.h"

namespace Picto
{
PredicateGreaterThan::PredicateGreaterThan()
: Predicate()
{
	predicateString_= ">";
	name_ = "Greater than";

	//draw the icon
	const QPoint points[] = { QPoint(25,30),
							   QPoint(75,50),
							   QPoint(25,70)};

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

bool PredicateGreaterThan::evaluate(double LHS, double RHS)
{
	return (LHS > RHS);
}

} //namespacePicto