#ifndef _NUMERIC_PARAMETER_H_
#define _NUMERIC_PARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*!	\brief A parameter for containing numeric values.
 *
 *	Given our experience with Picto, it seems reasonable to limit these
 *	parameters to integer values.  If a user requires a non-integer
 *	value (e.g. 0.135 seconds), they can usually get away with changing
 *	the units (e.g. 135 milliseconds).
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API NumericParameter : public Parameter
#else
class NumericParameter : public Parameter
#endif
{
	Q_OBJECT
public slots:
	void setValue(QVariant value);
	QVariant getValue() { return QVariant(value_); };

public:
	NumericParameter();

	static Parameter* NewParameter();
	static QSharedPointer<Serializable> Create(){return QSharedPointer<Serializable>(new NumericParameter());};
	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);


	void setUnits(QString units) { units_ = units; };

	QString getUnits() { return units_; };

	//note that the lessThan & greaterThan functions aren't redefined, 
	//so they will always return false
	bool greaterThan(Parameter& RHS);
	bool equalTo(Parameter& RHS);
	bool lessThan(Parameter& RHS);

	bool greaterThan(QVariant& RHS);
	bool equalTo(QVariant& RHS);
	bool lessThan(QVariant& RHS);

protected:
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	int value_;
	QString units_;
};


}; //namespace Picto

#endif
