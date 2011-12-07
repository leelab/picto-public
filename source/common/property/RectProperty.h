#ifndef _RECTPROPERTY_H_
#define _RECTPROPERTY_H_

#include "Property.h"

namespace Picto {
/*!	\brief Describes a RectProperty of an object
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API RectProperty : public Property
#else
class RectProperty : public Property
#endif
{
	Q_OBJECT
public:
	virtual ~RectProperty(){};
public slots:
	void set(int x, int y, int width, int height){Property::setValue(QRect(x,y,width,height));};
protected:
	RectProperty(QtVariantProperty* variantProp, QtVariantPropertyManager* manager);
	virtual void UpdateSerializationAttributesFromValue();
	virtual bool SetValueFromString(QVariant _value, QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual QString toUserString();
	virtual void fromUserString(QString userString);
private:
	void setX(QVariant x) {set(x.toInt(),value().toRect().y(),value().toRect().width(),value().toRect().height());};
	void setY(QVariant y) {set(value().toRect().x(),y.toInt(),value().toRect().width(),value().toRect().height());};
	void setWidth(QVariant w) {set(value().toRect().x(),value().toRect().y(),w.toInt(),value().toRect().height());};
	void setHeight(QVariant h) {set(value().toRect().x(),value().toRect().y(),value().toRect().width(),h.toInt());};
	QVariant getX() {return (QVariant)value().toRect().x();};
	QVariant getY() {return (QVariant)value().toRect().y();};
	QVariant getWidth() {return (QVariant)value().toRect().width();};
	QVariant getHeight() {return (QVariant)value().toRect().height();};
	friend class PropertyContainer;
};


}; //namespace Picto

#endif
