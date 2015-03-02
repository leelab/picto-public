#ifndef _ARROWGRAPHIC_H_
#define _ARROWGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief An arrow graphic.  This graphic is not currently used in Picto, so we won't be documenting it in detail.  Original documentation is included in the details.
 *
 *	An ArrowGraphic has the following properties:
 *	- Position: the position of the top-left corner of the box containing the arrow (in screen coordinates)
 *	- Start: The start position of the arrow (in local coordinates)
 *	- End: The end position of the arrow (in local coordinates)
 *	- Size: The size of the arrowhead
 *	- Color: The color
 *
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class PICTOLIB_CLASS ArrowGraphic : public VisualElement
{
	Q_OBJECT
	Q_PROPERTY(int endX READ getEndX WRITE setEndX)
	Q_PROPERTY(int endY READ getEndY WRITE setEndY)
	Q_PROPERTY(int thickness READ getThickness WRITE setThickness)

public:
	ArrowGraphic(QPoint start=QPoint(), QPoint end=QPoint(),
				int size=0, QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();
	static const QString type;

	QPoint getStartPoint() { return propertyContainer_->getPropertyValue("Start").toPoint(); };
	QPoint getEndPoint() { return propertyContainer_->getPropertyValue("End").toPoint(); };

	void setStartPoint(QPoint point) { propertyContainer_->setPropertyValue("Start", point);};
	void setEndPoint(QPoint point) { propertyContainer_->setPropertyValue("End", point);};

	int getThickness(){return propertyContainer_->getPropertyValue("Thickness").toInt();};
	void setThickness(int size) { propertyContainer_->setPropertyValue("Thickness", size);};
	int getEndX(){return getEndPoint().x();};
	void setEndX(int x){setEndPoint(QPoint(x,getEndPoint().y()));};
	int getEndY(){return getEndPoint().y();};
	void setEndY(int y){setEndPoint(QPoint(getEndPoint().x(),y));};

	virtual QString friendlyTypeName(){return "Arrow";};
public slots:
	void setEndPoint(int x, int y) { setEndPoint(QPoint(x,y));};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private slots:
	
};


}; //namespace Picto

#endif
