#ifndef _SCRIPT_ELEMENT_H_
#define _SCRIPT_ELEMENT_H_

#include <QSharedPointer>
#include <QCoreApplication>

#include "../common.h"
#include "StateMachineElement.h"
#include "../engine/PictoEngine.h"

namespace Picto {

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

#if defined WIN32 || defined WINCE
class PICTOLIB_API ScriptElement : public StateMachineElement
#else
class ScriptElement : public StateMachineElement
#endif
{
public:
	ScriptElement();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new ScriptElement());};

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	virtual QString runAsSlave(QSharedPointer<Engine::PictoEngine> engine);

	void setScript(QString script);

	//bool initScripting(QScriptEngine &qsEngine);
	virtual QString assetType(){return "ScriptElement";};

	//DataStore functions
//	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
//	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool hasScripts();
	//This returns a map of QMap<script name,script code>
	virtual QMap<QString,QString> getScripts();

};


}; //namespace Picto

#endif
