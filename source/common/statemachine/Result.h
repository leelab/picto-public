#ifndef _RESULT_H_
#define _RESULT_H_

#include "../common.h"
#include "../statemachine/ScriptableContainer.h"

namespace Picto {

/*!	\brief A Result is the control flow exit point from every element of a Picto StateMachine.
 *	\details In Picto, the StateMachine contains multiple levels.  Regardless of whether the
 *	particular level of a StateMachine is another StateMachine or a State or an element
 *	who's internal logic is defined by C++ code, control flow always leaves that element
 *	and returns to the level above through a Result element.  
 *
 *	Unlike other StateMachineElement objects, since Results have no internal logic, they
 *	have only an EntryScript and no ExitScript.  When exactly their EntryScript is called is of 
 *	course a function of the Result's parent container since the Result does not have its
 *	own run() function.  With that said, however, all elements that call a results EntryScript 
 *	(runResultScript()) should do so before calling their own ExitScript.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API Result : public ScriptableContainer
#else
class Result : public ScriptableContainer
#endif
{
	Q_OBJECT
public:
	Result();
	Result(QString name);
	virtual ~Result(){};

	static QSharedPointer<Asset> Create();

	virtual QString getUITemplate(){return "Result";};
	virtual QString friendlyTypeName(){return "Result";};
	virtual QString getUIGroup(){return "State Machine Elements";};
	virtual void runResultScript();

protected:
	virtual QString defaultTagName(){return "Result";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool hasScripts();
	virtual QMap<QString,QString> getScripts();
	virtual bool canHaveScripts(){return true;};

};


}; //namespace Picto

#endif
