#include <QPainter>
#include <QPoint>

#include "PredicateLessThanEq.h"

namespace Picto
{
PredicateLessThanEq::PredicateLessThanEq()
: Predicate()
{
	predicateString_= "<=";
	name_ = "Less than or equal to";

	//draw the icon
	const QPoint pointsLT[] = { QPoint(75,30),
							   QPoint(25,50),
							   QPoint(75,70)};
	const QPoint pointsEq[] = { QPoint(22,60),
							   QPoint(70,80)};

	QPainter p(&icon_);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(QColor(0,0,0,255));

	QPen pen(QColor(0,0,0,255));
	pen.setWidth(4);
	pen.setJoinStyle(Qt::MiterJoin);
	p.setPen(pen);

	p.drawPolyline(pointsLT,3);
	p.drawPolyline(pointsEq,2);
	p.end();


}

bool PredicateLessThanEq::evaluate(double LHS, double RHS)
{
	return (LHS <= RHS);
}

} //namespacePicto