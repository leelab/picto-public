#ifndef _RESULT_H_
#define _RESULT_H_

#include "../common.h"
#include "../statemachine/ScriptableContainer.h"

namespace Picto {

/*!	\brief A Result is the exit point from a StateMachineElement or ControlElement
 *	
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
	/*! \brief Constructs new result with input name and option to define result as default.
	 *	Define the result as default if it is added automatically by your ResultContainer and
	 *	need not be serialized out.
	 */
	Result(QString name);
	virtual ~Result(){};

	static QSharedPointer<Asset> Create();

	virtual QString getUITemplate(){return "Result";};
	virtual QString friendlyTypeName(){return "Result";};
	virtual QString getUIGroup(){return "State Machine Elements";};
	void runResultScript();

protected:
	virtual QString defaultTagName(){return "Result";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool hasScripts();
	virtual QMap<QString,QPair<QString,QString>> getScripts();
	virtual bool canHaveScripts(){return true;};

	QSharedPointer<AssetFactory> resultScriptFactory_;

};


}; //namespace Picto

#endif
