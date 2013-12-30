#ifndef _SCRIPT_ELEMENT_H_
#define _SCRIPT_ELEMENT_H_

#include <QSharedPointer>
#include <QCoreApplication>

#include "../common.h"
#include "StateMachineElement.h"
#include "../engine/PictoEngine.h"

namespace Picto {

/*!	\brief This class is obsolete. In design syntax version "0.0.1", we removed "ScriptElements" and replaced them with "SwitchElements" since "ScriptElement" were essentially 
 *	"SwitchElements" that contained only two results, "true" and "false" that were triggered with a boolean value.
 *	\details We should probably look into deleting this class.  For reference, I have included the classes original documentation below:
 *
 *	ScriptElement: A state machine element that runs a script
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
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API ScriptElement : public StateMachineElement
#else
class ScriptElement : public StateMachineElement
#endif
{
public:
	ScriptElement();
	virtual ~ScriptElement(){};
	static QSharedPointer<Asset> Create();

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	virtual QString slaveRun(QSharedPointer<Engine::PictoEngine> engine);

	void setScript(QString script);

	virtual QString getUITemplate(){return "ScriptElement";};
	virtual QString friendlyTypeName(){return "Script Element";};

	//DataStore functions
//	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
//	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();
	//This returns a map of QMap<script name,script code>
	virtual QMap<QString,QString>  getScripts();

};


}; //namespace Picto

#endif
