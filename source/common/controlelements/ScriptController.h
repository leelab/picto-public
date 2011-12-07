#ifndef _SCRIPT_CONTROLLER_H_
#define _SCRIPT_CONTROLLER_H_

#include "../common.h"
#include "ControlElement.h"

namespace Picto {

/*! \brief A time based controller
 *
 *	This controller ignores any and all inputs, and simply returns "Success"
 *	when a fixed amount of time has expired.  Since the time values are properties
 *	it is possible to change them at runtime through Javascript.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API ScriptController : public ControlElement
#else
class ScriptController : public ControlElement
#endif
{
	Q_OBJECT
public:
	ScriptController();
	virtual ~ScriptController(){};

	static ControlElement* NewScriptController();
	static QSharedPointer<Asset> Create();
	static QString ControllerType();

	bool isDone(QSharedPointer<Engine::PictoEngine> engine);
	QString getResult();
	void start(QSharedPointer<Engine::PictoEngine> engine);


	////DataStore Functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();
	//This returns a map of QMap<script name,script code>
	virtual QMap<QString,QString> getScripts();

private:
};


}; //namespace Picto

#endif
