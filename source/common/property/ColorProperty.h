#ifndef _COLORPROPERTY_H_
#define _COLORPROPERTY_H_

#include "Property.h"

namespace Picto {
/*!	\brief Describes a RectProperty of an object
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API ColorProperty : public Property
#else
class ColorProperty : public Property
#endif
{
	Q_OBJECT
public:
	virtual QString toUserString();
	virtual void fromUserString(QString userString);
public slots:
	void set(int r, int g, int b, int a=255){setColor(QColor(r,g,b,a));};
protected:
	ColorProperty(QSharedPointer<QtVariantProperty> variantProp, QSharedPointer<QtVariantPropertyManager> manager);
	virtual void UpdateSerializationAttributesFromValue();
	virtual bool SetValueFromString(QVariant _value, QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QColor getColor(){return value().value<QColor>();};
	void setColor(QColor color){setValue(QVariant(color));};
	QVariant getRed() { return (QVariant)getColor().red(); };
	QVariant getGreen() { return (QVariant)getColor().green(); };
	QVariant getBlue() { return (QVariant)getColor().blue(); };
	QVariant getAlpha() { return (QVariant)getColor().alpha(); };
	void setRed(QVariant r){QColor val = getColor(); val.setRed(r.toInt());setColor(val);};
	void setGreen(QVariant g){QColor val = getColor(); val.setGreen(g.toInt());setColor(val);};
	void setBlue(QVariant b){QColor val = getColor(); val.setBlue(b.toInt());setColor(val);};
	void setAlpha(QVariant a){QColor val = getColor(); val.setAlpha(a.toInt());setColor(val);};
	friend class PropertyContainer;
};


}; //namespace Picto

#endif