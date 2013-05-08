#ifndef _ELLIPSEGRAPHIC_H_
#define _ELLIPSEGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief An ellipse graphic.
 *
 *	An EllipseGraphic has the following properties:
 *	- Position: The position of the top-left corner of the box containing the ellipse (in screen coordinates)
 *	- Dimensions: The width and height of the ellipse
 *	- Color: The color
 */

class PICTOLIB_CLASS EllipseGraphic : public VisualElement
{
	Q_OBJECT
	Q_PROPERTY(int width READ getWidth WRITE setWidth)
	Q_PROPERTY(int height READ getHeight WRITE setHeight)
	Q_PROPERTY(bool outline READ getOutline WRITE setOutline)

public:
	EllipseGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();
	QRect getDimensions(){ return QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize()); };
	void setDimensions(QRect dimensions){ propertyContainer_->setPropertyValue("Size",dimensions.size());};

	int getWidth(){return getDimensions().width();};
	int getHeight(){return getDimensions().height();};
	void setWidth(int w){QRect dims = getDimensions(); dims.setWidth(w);setDimensions(dims);};
	void setHeight(int h){QRect dims = getDimensions(); dims.setHeight(h);setDimensions(dims);};
	bool getOutline(){return propertyContainer_->getPropertyValue("Outline").toBool();};
	void setOutline(bool outline) { propertyContainer_->setPropertyValue("Outline", outline);};

	QPoint getPositionOffset();
	static const QString type;

	virtual QString friendlyTypeName(){return "Ellipse";};
public slots:
	void setDimensions(int w, int h){setWidth(w);setHeight(h);};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QPoint posOffset_;
private slots:
	
};


}; //namespace Picto

#endif
