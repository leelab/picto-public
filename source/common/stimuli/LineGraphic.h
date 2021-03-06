#ifndef _LINEGRAPHIC_H_
#define _LINEGRAPHIC_H_
#include <QVector>
#include <QPoint>

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief This class is not currently used in Picto.  The original documentation appears below:
 *	\details A line segment graphic.
 *
 *	A line graphic is basically a set of points (in the graphic's internal coordinates) that draw a 
 *	series of line segments.  This one is a bit weird, since the number of properties is indeterminate
 *	(it depends on the number of segments).
 *
 *	A LineGraphic has the following properties:
 *	- Position: The position of the top-left corner of the box containing the line (in screen coordinates)
 *	- Point 1 - N: The points (in order) that will be conencted to form the line graphic.
 *	- Color: The color of the lines
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PICTOLIB_CLASS LineGraphic : public VisualElement
{
	Q_OBJECT

public:
	LineGraphic(QPoint position=QPoint(), QVector<QPoint> = QVector<QPoint>(2,QPoint()), QColor color=QColor());

	void draw();
	static QSharedPointer<Asset> Create();
	static const QString type;

	virtual QString friendlyTypeName(){return "Line";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QVector<QPoint> points_;

private slots:
	
};


}; //namespace Picto

#endif
