#ifndef _Variable_H_
#define _Variable_H_

#include <QVariant>
#include <QScriptValue>

#include "../common.h"
#include "../statemachine/Scriptable.h"

namespace Picto {

/*!	\brief A base class for storing values that can be used in a state machine
 *
 *	Since Picto requires users to build state machines to define their experiments, we need
 *	some sort of "variable".
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API Variable : public Scriptable
#else
	class Variable : public Scriptable
#endif
{
	Q_OBJECT

public:

	Variable();
	virtual ~Variable(){};

	virtual QString getUITemplate(){return "NumberVariable";};
	virtual QString friendlyTypeName(){return "Variable";};
	virtual QString getUIGroup(){return "Variables";};

protected:
	virtual QString defaultTagName(){return "Variable";};
	virtual void postDeserialize();
};


}; //namespace Picto

#endif
