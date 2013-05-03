#ifndef _ARROWGRAPHIC_H_
#define _ARROWGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief An arrow graphic.
 *
 *	An ArrowGraphic has the following properties:
 *	- Position: the position of the top-left corner of the box containing the arrow (in screen coordinates)
 *	- Start: The start position of the arrow (in local coordinates)
 *	- End: The end position of the arrow (in local coordinates)
 *	- Size: The size of the arrowhead
 *	- Color: The color
 */
class PICTOLIB_CLASS ArrowGraphic : public VisualElement
{
	Q_OBJECT
	Q_PROPERTY(int endX READ getEndX WRITE setEndX)
	Q_PROPERTY(int endY READ getEndY WRITE setEndY)
	Q_PROPERTY(int headSize READ getHeadSize WRITE setHeadSize)

public:
	ArrowGraphic(QPoint start=QPoint(), QPoint end=QPoint(),
				int size=0, QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();

	QPoint getStartPoint() { return propertyContainer_->getPropertyValue("Start").toPoint(); };
	QPoint getEndPoint() { return propertyContainer_->getPropertyValue("End").toPoint(); };

	void setStartPoint(QPoint point) { propertyContainer_->setPropertyValue("Start", point);};
	void setEndPoint(QPoint point) { propertyContainer_->setPropertyValue("End", point);};

	int getHeadSize(){return propertyContainer_->getPropertyValue("HeadSize").toInt();};
	void setHeadSize(int size) { propertyContainer_->setPropertyValue("HeadSize", size);};
	int getEndX(){return getEndPoint().x();};
	void setEndX(int x){setEndPoint(QPoint(x,getEndPoint().y()));};
	int getEndY(){return getEndPoint().y();};
	void setEndY(int y){setEndPoint(QPoint(getEndPoint().x(),y));};

	static const QString type;

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
