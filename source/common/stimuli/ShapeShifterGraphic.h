#ifndef _SHAPESHIFTERGRAPHIC_H_
#define _SHAPESHIFTERGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief Represents a shape shifter graphic that can change its shape at runtime between Ellipse, Rectangle, and Diamond.
 *
 *	\details In addition to the Properties provided by the VisualElement class, this class provides size, outline and outline thickness
 *	Properties.  The size is just the total width and height of the graphic.  The outline Property determines if only the outline of the
 *	graphic will be drawn.  The outline thickness Property determines the thickness of the graphic outline if the outline Property is true.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PICTOLIB_CLASS ShapeShifterGraphic : public VisualElement
{
	Q_OBJECT
	/*! \brief Sets/gets the width of this graphic in pixels.*/
	Q_PROPERTY(int width READ getWidth WRITE setWidth)
	/*! \brief Sets/gets the height of this graphic in pixels.*/
	Q_PROPERTY(int height READ getHeight WRITE setHeight)
	/*! \brief Sets/gets the current shape of this graphic as a string ("Ellipse", "Rectangle" or "Diamond").*/
	Q_PROPERTY(QString shape READ getShape WRITE setShape)

public:
	ShapeShifterGraphic(QPoint position=QPoint(), QRect dimensions=QRect(), QColor color=QColor());

	void draw();
	static QSharedPointer<Asset> Create();
	static const QString type;

	QRect getDimensions();
	void setDimensions(QRect dimensions);
	/*! \brief Returns the width of this graphic in pixels.*/
	int getWidth(){return getDimensions().width();};
	/*! \brief Returns the height of this graphic in pixels.*/
	int getHeight(){return getDimensions().height();};
	/*! \brief Sets the width of this graphic in pixels.*/
	void setWidth(int w){QRect dims = getDimensions(); dims.setWidth(w);setDimensions(dims);};
	/*! \brief Sets the height of this graphic in pixels.*/
	void setHeight(int h){QRect dims = getDimensions(); dims.setHeight(h);setDimensions(dims);};
	QPoint getPositionOffset();
	QString getShape();
	void setShape(QString shape);

	virtual QString friendlyTypeName(){return "Shape Shift";};
	virtual QString getUITemplate(){return "ShapeShifterElement";};
public slots:
	/*! \brief Set the dimensions (width, height) of this graphic.*/
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
