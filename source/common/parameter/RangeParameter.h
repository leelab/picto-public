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
public slots:
	void setValue(QVariant value);
	QVariant getValue() { return QVariant(currentValue_); };

public:
	RangeParameter();

	static Parameter* NewParameter();
	static QSharedPointer<Serializable> Create(){return QSharedPointer<Serializable>(new RangeParameter());};
	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);


	void setMin(int min);
	void setMax(int max);
	void setDefault(int defualtValue);
	void setIncrement(int increment) {increment_ = increment; }
	void setUnits(QString units) { units_ = units; };

	void increment() { currentValue_ += increment_; };
	void decrement() { currentValue_ -= increment_; };

	bool greaterThan(Parameter& RHS) { return currentValue_ > RHS.getValue().toInt(); };
	bool lessThan(Parameter& RHS) { return currentValue_ < RHS.getValue().toInt(); };
	bool equalTo(Parameter& RHS) { return currentValue_ == RHS.getValue().toInt(); } ;

	bool greaterThan(QVariant& RHS)
	{
		bool ok;
		int RHSValue = RHS.toInt(&ok);
		return ok && currentValue_ > RHSValue; 
	};
	bool lessThan(QVariant& RHS)	
	{
		bool ok;
		int RHSValue = RHS.toInt(&ok);
		return ok && currentValue_ < RHSValue; 
	};

	bool equalTo(QVariant& RHS)	
	{
		bool ok;
		int RHSValue = RHS.toInt(&ok);
		return ok && currentValue_ == RHSValue; 
	};

protected:
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	int maxValue_;
	int minValue_;
	int defaultValue_;
	int currentValue_;
	int increment_;
	QString units_;

};


}; //namespace Picto

#endif
