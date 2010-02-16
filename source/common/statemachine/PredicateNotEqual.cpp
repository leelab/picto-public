#include <QPainter>
#include <QPoint>

#include "PredicateNotEqual.h"

namespace Picto
{
PredicateNotEqual::PredicateNotEqual()
: Predicate()
{
	predicateString_= "!=";
	name_ = "Not equal";

	//draw the icon

	QPainter p(&icon_);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(QColor(0,0,0,255));

	QPen pen(QColor(0,0,0,255));
	pen.setWidth(4);
	pen.setJoinStyle(Qt::MiterJoin);
	p.setPen(pen);

	p.drawLine(25,40,75,40);
	p.drawLine(25,60,75,60);
	p.drawLine(65,30,35,70);
	p.end();


}

bool PredicateNotEqual::evaluate(double LHS, double RHS)
{
	return (LHS != RHS);
}

} //namespacePicto