#ifndef _ColorParameter_H_
#define _ColorParameter_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*! \brief A boolean parameter.
 *
 *	This parameter can only hold the values true or false, although we can relable those
 *	values to anything we want (e.g. "On" and "Off").  The equality operators work as 
 *	expected for these parameters, but the greater than and less than operators will
 *	always return false, since theydoesn't make sense with boolean values.
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API ColorParameter : public Parameter
#else
class ColorParameter : public Parameter
#endif
{
	Q_OBJECT
	Q_PROPERTY(int red READ getRed WRITE setRed)
	Q_PROPERTY(int green READ getGreen WRITE setGreen)
	Q_PROPERTY(int blue READ getBlue WRITE setBlue)
	Q_PROPERTY(int alpha READ getAlpha WRITE setAlpha)
	Q_PROPERTY(QVariant value READ getColor WRITE setColor)

//public slots:
//	void setValue(QVariant value) { value_ = value.toBool(); };
//	QVariant getValue() { return QVariant(value_); };

public:
	ColorParameter();
	virtual ~ColorParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();

	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	QColor getColor(){return propertyContainer_->getPropertyValue("Value").value<QColor>();};
	void setColor(QVariant val){propertyContainer_->setPropertyValue("Value",val.value<QColor>());};
	int getRed() { return getColor().red(); };
	int getGreen() { return getColor().green(); };
	int getBlue() { return getColor().blue(); };
	int getAlpha() { return getColor().alpha(); };
	void setRed(int r){QColor val = getColor(); val.setRed(r);setColor(val);};
	void setGreen(int g){QColor val = getColor(); val.setGreen(g);setColor(val);};
	void setBlue(int b){QColor val = getColor(); val.setBlue(b);setColor(val);};
	void setAlpha(int a){QColor val = getColor(); val.setAlpha(a);setColor(val);};

	virtual QString getUITemplate(){return "ColorParameter";};
	virtual QString friendlyTypeName(){return "Color";};

public slots:
	void setColor(int r, int g, int b, int a=255){setColor(QColor(r,g,b,a));};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};


}; //namespace Picto

#endif
