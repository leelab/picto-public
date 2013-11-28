#ifndef _ColorParameter_H_
#define _ColorParameter_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*! \brief A simple parameter for holding a color value.
 *
 *	\details The Value Property is runtime editable and appears as a color selection widget with 3 editable values and a color wheel pop-up
 *	in the PropertyFrame.  The color value is accessible through various javascript properties and functions.  For example:
 *	\code
 		ColorParameterName.value = "#FF2255";	//"#rrggbb"
 		fixationPointer.red = ColorParameterName.red;
 	\endcode
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API ColorParameter : public Parameter
#else
class ColorParameter : public Parameter
#endif
{
	Q_OBJECT
	/*! \brief Sets/Gets the red component of the color (0-255).*/
	Q_PROPERTY(int red READ getRed WRITE setRed)
	/*! \brief Sets/Gets the green component of the color (0-255).*/
	Q_PROPERTY(int green READ getGreen WRITE setGreen)
	/*! \brief Sets/Gets the blue component of the color (0-255).*/
	Q_PROPERTY(int blue READ getBlue WRITE setBlue)
	/*! \brief Sets/Gets the transparency of the color (0-255).*/
	Q_PROPERTY(int alpha READ getAlpha WRITE setAlpha)
	/*! \brief Sets/Gets the the color as a hexadecimal r,g,b value string "#rrggbb".  For example, green would be "#00FF00".*/
	Q_PROPERTY(QVariant value READ getColor WRITE setColor)

public:
	ColorParameter();
	virtual ~ColorParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();

	/*! \brief Returns the current color.*/
	QColor getColor(){return propertyContainer_->getPropertyValue("Value").value<QColor>();};
	/*! \brief Sets the current color.*/
	void setColor(QVariant val){propertyContainer_->setPropertyValue("Value",val.value<QColor>());};
	/*! \brief Returns the red component of the color (0-255).*/
	int getRed() { return getColor().red(); };
	/*! \brief Returns the green component of the color (0-255).*/
	int getGreen() { return getColor().green(); };
	/*! \brief Returns the blue component of the color (0-255).*/
	int getBlue() { return getColor().blue(); };
	/*! \brief Returns the transparency of the color (0-255).*/
	int getAlpha() { return getColor().alpha(); };
	/*! \brief Sets the red component of the color (0-255).*/
	void setRed(int r){QColor val = getColor(); val.setRed(r);setColor(val);};
	/*! \brief Sets the green component of the color (0-255).*/
	void setGreen(int g){QColor val = getColor(); val.setGreen(g);setColor(val);};
	/*! \brief Sets the blue component of the color (0-255).*/
	void setBlue(int b){QColor val = getColor(); val.setBlue(b);setColor(val);};
	/*! \brief Sets the transparency of the color (0-255).*/
	void setAlpha(int a){QColor val = getColor(); val.setAlpha(a);setColor(val);};

	virtual QString getUITemplate(){return "ColorParameter";};
	virtual QString friendlyTypeName(){return "Color";};

public slots:
	/*! \brief Sets the current color according to the input red, green, blue, alpha components, each of which must be between 0-255.*/
	void setColor(int r, int g, int b, int a=255){setColor(QColor(r,g,b,a));};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};


}; //namespace Picto

#endif
