#ifndef _NUMERIC_VALUE_H_
#define _NUMERIC_VALUE_H_

#include "AnalysisTool.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API NumericVariable : public AnalysisTool
#else
class NumericVariable : public AnalysisTool
#endif
{
	Q_OBJECT
	Q_PROPERTY(double value READ getValue WRITE setValue)
public:
	NumericVariable();
	static QSharedPointer<Asset> Create();

	virtual ~NumericVariable();

	//Resets the NumericVariable Object to its initial state.
	virtual void reset();

	//Inherited
	virtual QString getUITemplate(){return "NumericVariable";};
	virtual QString friendlyTypeName(){return "NumericVariable";};

protected:

	double getValue(){return value_;};
	void setValue(double val){value_ = val;};
	//Inherited
	virtual QString defaultTagName(){return "NumericVariable";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	double value_;

};
}; //namespace Picto
#endif