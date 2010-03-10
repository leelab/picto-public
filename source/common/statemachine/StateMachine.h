/*!	\Brief A container for StateMachineElements that acts as a statemachine
 *
 *	A StateMachine contains multiple StateMachineElements and the
 *	transitions between them.  Additionally, a StateMachine is a 
 *	StateMachineElement, so it can be contained by other StateMachines.
 */
#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include "../common.h"
#include "StateMachineElement.h"
#include "Transition.h"

#include <QScriptEngine>

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API StateMachine : public StateMachineElement
#else
class StateMachine : public StateMachineElement
#endif
{
public:
	StateMachine();

	void addTransition(QSharedPointer<Transition> transition);
	void addParameter(QSharedPointer<Parameter> parameter);
	void addElement(QSharedPointer<StateMachineElement> element);
	bool setInitialElement(QString elementName);

	void setLevel(QString level);
	QString getLevel();

	bool validateTransitions();

	QString run();
	bool initScripting(QScriptEngine &qsEngine);

	//DataStore functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	QMultiMap<QString, QSharedPointer<Transition> > transitions_; //<source, transition>
	QMap<QString, QSharedPointer<StateMachineElement> > elements_;

	//This is used to keep track of the local parameters, so that I don't
	//serialize all the parameters every time.
	ParameterContainer localParameterContainer_;

	QScriptEngine qsEngine_;
	bool scriptingInit_;

};


}; //namespace Picto

#endif
