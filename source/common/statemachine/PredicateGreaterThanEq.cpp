#include <QPainter>
#include <QPoint>

#include "PredicateGreaterThanEq.h"

namespace Picto
{
PredicateGreaterThanEq::PredicateGreaterThanEq()
: Predicate()
{
	predicateString_= ">=";

	//draw the icon
	const QPoint pointsGT[] = { QPoint(25,30),
							   QPoint(75,50),
							   QPoint(25,70)};
	const QPoint pointsEq[] = { QPoint(28,80),
							 QPoint(80,60) };

	QPainter p(&icon_);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(QColor(0,0,0,255));

	QPen pen(QColor(0,0,0,255));
	pen.setWidth(4);
	pen.setJoinStyle(Qt::MiterJoin);
	p.setPen(pen);

	p.drawPolyline(pointsGT,3);
	p.drawPolyline(pointsEq,2);
	p.end();


}

bool PredicateGreaterThanEq::evaluate(double LHS, double RHS)
{
	return (LHS >= RHS);
}

} //namespacePicto