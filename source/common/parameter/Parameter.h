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

	Parameter();

	//void setInitialValue(QVariant val);
	//virtual void setValue(QVariant value);
	//virtual QVariant getValue();

	virtual QString getUITemplate(){return "Parameter";};
	//QString type();
	virtual QString assetType(){return "Parameter";};

public slots:
	//setters and getters are slots so we can bind them to scripts


protected:
	//Returns false if something got changed.  If a warning should be issued, it goes in QString& warning.
	//If true is returned warning will not be checked.
	virtual bool fixValues(QString&){return true;};
	virtual QString defaultTagName(){return "Parameter";};
	virtual void postSerialize();
	QVariant currValue_;
private:
	bool fixingValues_;
private slots:
	void valueEdited();
};


}; //namespace Picto

#endif
