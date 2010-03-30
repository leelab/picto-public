/*!	\brief A state machine element that runs a script
 *
 *	Script elements are used when a script needs to be run.  The script
 *	being run will have access to all of the parameters of the state machine 
 *	that contains the ScriptElement.
 *
 *	There are some additional features that I have considered adding:
 *	 1. Multiple results.  At the moment, the ScriptElement has a single
 *		return value of "done"
 *	 2. Access to more than just the parameters.  It might be interesting to
 *		provide the script access to the VisualElements contained in Scenes.
 */
#ifndef _SCRIPT_ELEMENT_H_
#define _SCRIPT_ELEMENT_H_

#include <QSharedPointer>
#include <QScriptEngine>

#include "../common.h"
#include "StateMachineElement.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API ScriptElement : public StateMachineElement
#else
class ScriptElement : public StateMachineElement
#endif
{
public:
	ScriptElement();
	
	QString run();

	void setScript(QString script);

	bool initScripting(QScriptEngine &qsEngine);

	//DataStore functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QScriptEngine *qsEngine_;

};


}; //namespace Picto

#endif