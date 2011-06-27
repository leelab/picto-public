#ifndef _LINEGRAPHIC_H_
#define _LINEGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

#include <QVector>
#include <QPoint>

namespace Picto {

/*!	\brief A line segment graphic.
 *
 *	A line graphic is basically a set of points (in the graphic's internal coordinates) that draw a 
 *	series of line segments.  This one is a bit weird, since the number of properties is indeterminate
 *	(it depends on the number of segments).
 *
 *	A LineGraphic has the following properties:
 *	- Position: The position of the top-left corner of the box containing the line (in screen coordinates)
 *	- Point 1 - N: The points (in order) that will be conencted to form the line graphic.
 *	- Color: The color of the lines
 */

class PICTOLIB_CLASS LineGraphic : public VisualElement
{
	Q_OBJECT

public:
	LineGraphic(QPoint position=QPoint(), QVector<QPoint> = QVector<QPoint>(2,QPoint()), QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new LineGraphic());};
	static const QString type;

	//! \TODO At some point, we might want to expose some getter and setter slots for script binding

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QVector<QPoint> points_;

private slots:
	void slotPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);
};


}; //namespace Picto

#endif
