#ifndef _SCRIPTABLE_H
#define _SCRIPTABLE_H

#include <QVariant>
#include <QScriptEngine>
#include <QScriptValue>

#include "../common.h"
#include "../statemachine/UIEnabled.h"

namespace Picto {

/*!	\brief A base class for classes that can be used in scripts
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API Scriptable : public UIEnabled
#else
	class Scriptable : public UIEnabled
#endif
{
	Q_OBJECT

public:

	Scriptable();

	bool bindToScriptEngine(QScriptEngine &engine);

protected:
	virtual QString defaultTagName(){return "Scriptable";};
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
