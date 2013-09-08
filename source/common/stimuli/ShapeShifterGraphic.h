#ifndef _SHAPESHIFTERGRAPHIC_H_
#define _SHAPESHIFTERGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

	/*!	\brief A box graphic (a rectangle).
 *
 *	A ShapeShifterGraphic has the following properties:
 *	- Position: the position of the middle corner of the box (in screen coordinates)
 *	- Dimensions: the size of the rectangle
 *	- Color: The color of the rectangle
 */

class PICTOLIB_CLASS ShapeShifterGraphic : public VisualElement
{
	Q_OBJECT
	Q_PROPERTY(int width READ getWidth WRITE setWidth)
	Q_PROPERTY(int height READ getHeight WRITE setHeight)
	Q_PROPERTY(QString shape READ getShape WRITE setShape)

public:
	ShapeShifterGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), QColor color=QColor());

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();

	QRect getDimensions();
	void setDimensions(QRect dimensions);
	int getWidth(){return getDimensions().width();};
	int getHeight(){return getDimensions().height();};
	void setWidth(int w){QRect dims = getDimensions(); dims.setWidth(w);setDimensions(dims);};
	void setHeight(int h){QRect dims = getDimensions(); dims.setHeight(h);setDimensions(dims);};
	QPoint getPositionOffset();
	QString getShape();
	void setShape(QString shape);

	static const QString type;

	virtual QString friendlyTypeName(){return "Shape Shift";};
	virtual QString getUITemplate(){return "ShapeShifterElement";};
public slots:
	void setDimensions(int w, int h){setWidth(w);setHeight(h);};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QPoint posOffset_;
	QStringList shapeList_;
private slots:
	
};


}; //namespace Picto

#endif
