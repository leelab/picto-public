#ifndef _SCRIPTABLE_H
#define _SCRIPTABLE_H

#include <QVariant>
#include <QScriptEngine>
#include <QScriptValue>

#include "../common.h"
#include "../statemachine/UIEnabled.h"

namespace Picto {

/*!	\brief A base class for classes that can be used in scripts
 *	\details Scriptable elements are elements that need to be accessible from javascript in the Picto design.
 *	Any script in a Picto Design can access Scriptable elements that are the direct children of any of their ancestor
 *	elements.  In javascript Scriptable elements are accessed just like any other javascript object. ie.
 *	\code
		scriptableElement.value = "new value";
		scriptableElement.someFunction(param1,param2);
	\endcode
 *	\details Adding script properties (ie. "value" in the example above) and script functions (ie. someFunction() in the
 *	example above) is straightforward.  Every Q_PROPERTY in a class that inherits from Scriptable will appear as a script
 *	property in the script environment.  Every public Slot in a class that inherits from Scriptable will appear as a
 *	script function that can take inputs and return values.  C++ types are generally mapped in the most logical way to
 *	javascript types; however, it is easiest to think of the javascript types as more or less the same as a QVariant.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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

	virtual bool hasContentsForScriptEngine();
	virtual bool bindToScriptEngine(QScriptEngine &engine);
	/*! \brief Resets this scriptable's data fields to their original values.
	 */
	virtual void enteredScope();
	virtual QString getUITemplate(){return "Scriptable";};
	/*! brief Returns true if a widget for adjusting this element's runtime editable Properties' initValues will show up
	 *	in TestViewer and RemoteViewer UIs.
	 */
	bool isUIEnabled(){return propertyContainer_->getPropertyValue("UIEnabled").toBool();};
	/*! \brief This is not currently used, however the idea is that at somepoint the designer should be able to somehow
	 *	set the order in which UIEnabled elements appear in widget boxes.  This would store this elements position in the
	 *	widget box.
	 *	\sa isUIEnabled(), isRuntimeEditable()
	 */
	int getUIOrder(){return propertyContainer_->getPropertyValue("UIOrder").toInt();};
	virtual bool isRuntimeEditable(){return isUIEnabled();};
	virtual void setPropertyRuntimeEditable(QString propName, bool editable = true);
	virtual QString getScriptingInfo();
	QString getInfo();

protected:
	virtual QString defaultTagName(){return "Scriptable";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void restoreProperties();
};


}; //namespace Picto

#endif
