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
	virtual ~Scriptable(){};

	virtual bool bindToScriptEngine(QScriptEngine &engine);
	/*! \brief Resets this scriptables data fields to their original values.
	 */
	virtual void reset();
	virtual QString getUITemplate(){return "Scriptable";};
	bool isUIEnabled(){return propertyContainer_->getPropertyValue("UIEnabled").toBool();};
	int getUIOrder(){return propertyContainer_->getPropertyValue("UIOrder").toInt();};
	virtual bool isRuntimeEditable(){return isUIEnabled();};
	virtual void setPropertyRuntimeEditable(QString propName, bool editable = true);
	virtual QString getScriptingInfo();
	QString getInfo();

protected:
	//\brief Overidden to include script code that should be injected into the script engine
	virtual QString scriptCode(){return "";};
	virtual QString defaultTagName(){return "Scriptable";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void restoreProperties();
};


}; //namespace Picto

#endif
