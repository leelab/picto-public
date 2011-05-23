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

public:
	ArrowGraphic(QPoint position=QPoint(), QPoint start=QPoint(), QPoint end=QPoint(),
				int size=0, QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Serializable> Create(){return QSharedPointer<Serializable>(new ArrowGraphic());};

	QPoint getStartPoint() { return propertyContainer_->getPropertyValue("Start").toPoint(); };
	QPoint getEndPoint() { return propertyContainer_->getPropertyValue("End").toPoint(); };

	void setStartPoint(QPoint point) { propertyContainer_->setPropertyValue("Start", point);};
	void setEndPoint(QPoint point) { propertyContainer_->setPropertyValue("End", point);};

	static const QString type;

public slots:
	int getStartPointX() { return getStartPoint().x(); };
	int getStartPointY() { return getStartPoint().y(); };
	void setStartPoint(int x, int y) { setStartPoint(QPoint(x,y)); };
	int getEndPointX() { return getEndPoint().x(); };
	int getEndPointY() { return getEndPoint().y(); };
	void setEndPoint(int x, int y) { setEndPoint(QPoint(x,y)); };
	int getSize() {return propertyContainer_->getPropertyValue("Size").toInt(); };
	void setSize(int size) { propertyContainer_->setPropertyValue("Size", size);};

protected:
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private slots:
	void slotPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);
};


}; //namespace Picto

#endif
