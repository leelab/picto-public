#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#include <QVariant>
#include <QScriptValue>

#include "../common.h"
#include "../statemachine/Scriptable.h"

namespace Picto {

/*!	\brief A base class for storing values that can be used in a state machine
 *
 *	Since Picto requires users to build state machines to define their experiments, we need
 *	some sort of "variable" (actually we don't, but variables make the state machines a lot
 *	simpler).  The Parameter object is that variable.  The base class includes functions
 *	for converting parameters to XML, as well as the comparison operators as virtual 
 *	functions.
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API Parameter : public Scriptable
#else
	class Parameter : public Scriptable
#endif
{
	Q_OBJECT

public:

	Parameter();
	virtual ~Parameter(){};

	//void setInitialValue(QVariant val);
	//virtual void setValue(QVariant value);
	//virtual QVariant getValue();

	virtual QString getUITemplate(){return "Parameter";};
	//QString type();
	virtual QString friendlyTypeName(){return "Parameter";};

	//Returns true if the parameter values are valid, false otherwise.
	//If false is returned, a warning message is placed in the warning input.
	virtual bool valuesAreValid(QString& warning = QString());
	//Fixes input values to make them valid again.
	virtual void fixValues();

public slots:
	//setters and getters are slots so we can bind them to scripts


protected:
	virtual QString defaultTagName(){return "Parameter";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QVariant currValue_;
private:
	bool fixingValues_;
};


}; //namespace Picto

#endif
