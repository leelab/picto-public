#ifndef _RANGEPARAMETER_H_
#define _RANGEPARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*!	\brief A Parameter that holds integer values within a limited range
 *
 *	The increment value is used throughtout the range parameter, and will
 *	result in strange behavior, if you aren't thinking about it.  The increment
 *	limits values to even multiples, so if the increment is 5, and you call
 *	setValue(312), the actual value that gets set is 310.  This also affects the
 *	min and max values.
 *
 *	At the moment, this Parameter type is limited to integer values.  This
 *	worked in the Orion system, so it seems likely to work here.  If it
 *	starts to cause problems, we can either subclass RangeParameter 
 *	(e.g. FloatRangeParameter), or we can use QVariants.
 *
 *	This is a somewhat redundant parameter type (since you could just use the 
 *	NumericParameter), but it seems like something that users will appreciate.
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API RangeParameter : public Parameter
#else
class RangeParameter : public Parameter
#endif
{
	Q_OBJECT
	Q_PROPERTY(int value READ getValue WRITE setValue)

//public slots:
//	void setValue(QVariant value);
//	QVariant getValue() { return QVariant(currentValue_); };

public:
	RangeParameter();
	virtual ~RangeParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();
	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);


	void setMin(int min);
	void setMax(int max);
	void setIncrement(int increment) {increment_ = increment; }
	void setUnits(QString units) { units_ = units; };
	int getValue(){return propertyContainer_->getPropertyValue("Value").toInt();};
	void setValue(int val){propertyContainer_->setPropertyValue("Value",val);};

	void increment() { currentValue_ += increment_; };
	void decrement() { currentValue_ -= increment_; };

	virtual QString friendlyTypeName(){return "Range";};
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool fixValues(QString& warning);

private:
	void updateFromProperties();
	int maxValue_;
	int minValue_;
	int currentValue_;
	int increment_;
	int value_;
	QString units_;

};


}; //namespace Picto

#endif
