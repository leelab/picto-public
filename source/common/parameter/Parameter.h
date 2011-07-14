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
	class Parameter : public UIEnabled
#endif
{
	Q_OBJECT

public:

	Parameter(int type);

	void setOrder(int i) { order_ = i; };
	int getOrder() { return order_; };
	void reset();
	void setInitialValue(QVariant val);

	//comparators
	//Note, all comparators, work with the current parameter value on the LHS
	//and the passed in constant on the RHS.  Also, if a comparator doesn't
	//make sense for a type of parameter, it doesn't need to be implemented
	//in the base class, and will simply return false

	//Note that we also need to deal with constants in our comparator functiosn

	//">", "<" and "==" need to be overridden in all derived classes, unless the
	//operation doesn't make sense, in which case we can simply let them
	//return false
	virtual bool greaterThan(Parameter& RHS) { Q_UNUSED(RHS); return false; };
	virtual bool equalTo(Parameter& RHS) { Q_UNUSED(RHS); return false; };
	virtual bool lessThan(Parameter& RHS) { Q_UNUSED(RHS); return false; };
	virtual bool greaterThan(QVariant& RHS) { Q_UNUSED(RHS); return false; };
	virtual bool equalTo(QVariant& RHS) { Q_UNUSED(RHS); return false; };
	virtual bool lessThan(QVariant& RHS) { Q_UNUSED(RHS); return false; };

	bool greaterThanEq(Parameter& RHS) { return greaterThan(RHS) || equalTo(RHS); };
	bool lessThanEq(Parameter& RHS) { return lessThan(RHS) || equalTo(RHS); };
	bool notEqualTo(Parameter& RHS) { return !equalTo(RHS); };
	bool greaterThanEq(QVariant& RHS) { return greaterThan(RHS) || equalTo(RHS); };
	bool lessThanEq(QVariant& RHS) { return lessThan(RHS) || equalTo(RHS); };
	bool notEqualTo(QVariant& RHS) { return !equalTo(RHS); };

	//operators (these call the named functions, but are included to simplify code)
	bool operator>(Parameter& RHS) { return greaterThan(RHS); };
	bool operator==(Parameter& RHS) { return equalTo(RHS); };
	bool operator>=(Parameter& RHS) { return greaterThanEq(RHS); };
	bool operator<(Parameter& RHS) { return lessThan(RHS); };
	bool operator<=(Parameter& RHS) { return lessThanEq(RHS); };
	bool operator!=(Parameter& RHS) { return notEqualTo(RHS); };

	bool operator>(QVariant& RHS) { return greaterThan(RHS); };
	bool operator==(QVariant& RHS) { return equalTo(RHS); };
	bool operator>=(QVariant& RHS) { return greaterThanEq(RHS); };
	bool operator<(QVariant& RHS) { return lessThan(RHS); };
	bool operator<=(QVariant& RHS) { return lessThanEq(RHS); };
	bool operator!=(QVariant& RHS) { return notEqualTo(RHS); };


	QString type();
	virtual QString assetType(){return "Parameter";};

public slots:
	//setters and getters are slots so we can bind them to scripts
	virtual void setValue(QVariant value);
	virtual QVariant getValue();


protected:
	virtual QVariant verifyValue(QVariant value){return value;};
	virtual QString defaultTagName(){return "Parameter";};
	virtual void postSerialize();
	QString type_;
	bool bOperatorUI_;
	int order_;
	QVariant currValue_;
};


}; //namespace Picto

#endif
