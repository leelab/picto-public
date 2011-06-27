#ifndef _BOXGRAPHIC_H_
#define _BOXGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

	/*!	\brief A box graphic (a rectangle).
 *
 *	A BoxGraphic has the following properties:
 *	- Position: the position of the top-left corner of the box (in screen coordinates)
 *	- Dimensions: the size of the rectangle
 *	- Color: The color of the rectangle
 */

class PICTOLIB_CLASS BoxGraphic : public VisualElement
{
	Q_OBJECT

public:
	BoxGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new BoxGraphic());};

	QRect getDimensions();
	void setDimensions(QRect dimensions);

	static const QString type;

public slots:
	int getWidth() { return getDimensions().width(); };
	void setWidth(int width);
	int getHeight() { return getDimensions().height(); };
	void setHeight(int height);

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private slots:
	void slotPropertyValueChanged(QString propertyName, int index, QVariant propertyValue);
};


}; //namespace Picto

#endif
