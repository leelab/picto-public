#ifndef _GRIDGRAPHIC_H_
#define _GRIDGRAPHIC_H_

#include <QVector>
#include <QPoint>

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief A line segment graphic.
 *
 *	A line graphic is basically a set of points (in the graphic's internal coordinates) that draw a 
 *	series of line segments.  This one is a bit weird, since the number of properties is indeterminate
 *	(it depends on the number of segments).
 *
 *	A GridGraphic has the following properties:
 *	- Position: The position of the top-left corner of the box containing the line (in screen coordinates)
 *	- Point 1 - N: The points (in order) that will be conencted to form the line graphic.
 *	- Color: The color of the lines
 */

class PICTOLIB_CLASS GridGraphic : public VisualElement
{
	Q_OBJECT
	Q_PROPERTY(int width READ getWidth WRITE setWidth)
	Q_PROPERTY(int height READ getHeight WRITE setHeight)
	Q_PROPERTY(int rows READ getRows WRITE setRows)
	Q_PROPERTY(int columns READ getColumns WRITE setColumns)
public:
	GridGraphic();

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();
	static const QString type;
	int getWidth(){return getDimensions().width();};
	int getHeight(){return getDimensions().height();};
	void setWidth(int w){QRect dims = getDimensions(); dims.setWidth(w);setDimensions(dims);};
	void setHeight(int h){QRect dims = getDimensions(); dims.setHeight(h);setDimensions(dims);};
	int getRows(){return propertyContainer_->getPropertyValue("Rows").toInt();};
	void setRows(int val){propertyContainer_->setPropertyValue("Rows",val);};
	int getColumns(){return propertyContainer_->getPropertyValue("Columns").toInt();};
	void setColumns(int val){propertyContainer_->setPropertyValue("Columns",val);};
	QRect getDimensions(){ return propertyContainer_->getPropertyValue("Dimensions").toRect(); };
	void setDimensions(QRect dimensions){ propertyContainer_->setPropertyValue("Dimensions",dimensions);};


	//! \TODO At some point, we might want to expose some getter and setter slots for script binding

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QVector<QPoint> points_;

private slots:
	//void slotPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);
};


}; //namespace Picto

#endif
