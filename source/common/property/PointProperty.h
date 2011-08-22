#ifndef _POINTPROPERTY_H_
#define _POINTPROPERTY_H_

#include "Property.h"

namespace Picto {
/*!	\brief Describes a RectProperty of an object
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API PointProperty : public Property
#else
class PointProperty : public Property
#endif
{
	Q_OBJECT
public slots:
	void set(int x, int y){Property::setValue(QPoint(x,y));};
protected:
	PointProperty(QSharedPointer<QtVariantProperty> variantProp, QSharedPointer<QtVariantPropertyManager> manager);
	virtual void UpdateSerializationAttributesFromValue();
	virtual bool SetValueFromString(QVariant _value, QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	void setX(QVariant x) {set(x.toInt(),value().toPoint().y());};
	void setY(QVariant y) {set(value().toPoint().x(),y.toInt());};
	QVariant getX() {return (QVariant)value().toPoint().x();};
	QVariant getY() {return (QVariant)value().toPoint().y();};
	friend class PropertyContainer;
};


}; //namespace Picto

#endif