#include <QDebug>
#include <QMetaProperty>
#include <QRegularExpression>
#include "Scriptable.h"
#include "../memleakdetect.h"
namespace Picto {

Scriptable::Scriptable()
{
}

bool Scriptable::hasContentsForScriptEngine()
{
	//If the object has no Q_PROPERTIES OR local public slots, don't add it.
	int numScriptableClassProps = Scriptable::staticMetaObject.propertyCount();
	int numScriptableClassFuncs = Scriptable::staticMetaObject.methodCount();
	const QMetaObject* metaObj = metaObject();
	int props = metaObj->propertyCount();
	int functions = metaObj->methodCount();
	if(numScriptableClassProps == props)
	{	//No Q_PROPERTIES to add.  Lets try public slots.
		bool hasPubSlots = false;
		for(int i = numScriptableClassFuncs;i<functions;i++)
		{
			QMetaMethod metaMeth = metaObj->method(i);
			if( (metaMeth.access() == QMetaMethod::Public)
				&& (metaMeth.methodType() == QMetaMethod::Slot) )
			{
				hasPubSlots = true;
				break;
			}
		}
		if(!hasPubSlots)
			return false;
	}
	return true;
}

bool Scriptable::bindToScriptEngine(QScriptEngine &engine)
{
	if(!hasContentsForScriptEngine())
		return true;
	Q_ASSERT(!getName().isEmpty());
	Q_ASSERT(!getName().contains(" "));

	QString myScriptValueName = getName().simplified().remove(' ');
	QScriptValue myScriptValue = engine.globalObject().property(myScriptValueName);
	
	if(!myScriptValue.isValid())
	{	//This ScriptValue hasn't been created yet.  Create it.
		myScriptValue = engine.newQObject(this,QScriptEngine::QtOwnership);
		engine.globalObject().setProperty(myScriptValueName,myScriptValue);
		//Check for errors
		if(engine.hasUncaughtException())
		{
			QString errorMsg = "Uncaught exception in Script Element " + getName() +"\n";
			errorMsg += QString("Line %1: %2\n").arg(engine.uncaughtExceptionLineNumber())
												.arg(engine.uncaughtException().toString());
			errorMsg += QString("Backtrace: %1\n").arg(engine.uncaughtExceptionBacktrace().join(", "));
			Q_ASSERT_X(!engine.hasUncaughtException(),"Scriptable::getScriptValue",errorMsg.toLatin1());
			return false;
		}
		Q_ASSERT(myScriptValue.isValid());
	}
	return true;
}

/*!	\brief Resets the VisualElement
 *
 *	Since Scriptables can be modified by scripts, resetting them returns them
 *	to their initial state.  This basically just reloads the PropertyContainer
 *	with the initial properties
 */
void Scriptable::enteredScope()
{
	restoreProperties();
}

void Scriptable::setPropertyRuntimeEditable(QString propName, bool editable)
{
	propertyContainer_->getProperty(propName)->setRuntimeEditable(editable);
}

QString Scriptable::getScriptingInfo()
{
	QString returnVal;

	//Since we don't want people to use the various signals and slots from Scriptable Superclasses,
	//but we DO want to enable hierarchical scripting property/function definitions, when we print
	//out the available scripts we ignore any properties and methods that are available in the
	//scriptable class, leaving all of those in its subclasses.
	//Since these functions are all still technically available to anyone that wants to use them,
	//this is a clear application security risk.  For now, we're not going to worry about that though.
	int numScriptableClassProps = Scriptable::staticMetaObject.propertyCount();
	int numScriptableClassFuncs = Scriptable::staticMetaObject.methodCount();


	//Print out the scriptable's properties, ignoring those that are in scriptable and its superclasses.
	const QMetaObject* metaObj = metaObject();
	Q_ASSERT(metaObj);
	int props = metaObj->propertyCount();
	QMetaProperty metaProp;
	for(int i=numScriptableClassProps;i<props;i++)
	{
		metaProp = metaObj->property(i);
		returnVal.append(QString("<dd><span style=\"color:blue;\">%1</span> - <span style=\"color:grey;\">(type:%2)</span></dd>").arg(metaProp.name()).arg(metaProp.typeName()));
	}

	//Now print out the functions that are available for scripting, ignoring those that are in 
	//scriptable and its superclasses.  Only display functions defined as public slots.
	int functions = metaObj->methodCount();
	QMetaMethod metaMeth;
	for(int i=numScriptableClassFuncs;i<functions;i++)
	{
		metaMeth = metaObj->method(i);
		if(metaMeth.access() != QMetaMethod::Public)
			continue;
		if(metaMeth.methodType() == QMetaMethod::Slot)
		{
			QString signature = metaMeth.methodSignature();
			QStringList commaSplitSig = signature.split(",");
			QList<QByteArray> paramNames = metaMeth.parameterNames();
			QStringList::iterator paramTypeIter = commaSplitSig.begin();
			QList<QByteArray>::iterator paramNameIter = paramNames.begin();
			//Color function name blue and the rest grey
			int openParenthLoc = (*paramTypeIter).indexOf("(");
			(*paramTypeIter).insert(openParenthLoc+1,"<span style=\"color:grey\">");
			(*paramTypeIter).append("</span>");
			while((paramTypeIter != commaSplitSig.end()) && (paramNameIter != paramNames.end()))
			{
				if(paramTypeIter != commaSplitSig.begin())
					(*paramTypeIter) = QString("<span style=\"color:grey\">%1</span>").arg(*paramTypeIter);
				(*paramTypeIter).replace("QString","String");	//We don't want QString in our UI, so we replace it with "String".
				(*paramTypeIter).append(QString(" <span style=\"color:blue\">%1</span>").arg(QString(*paramNameIter)));
				paramTypeIter++;
				paramNameIter++;
			}
			signature = commaSplitSig.join(",");
			//Move the last name in front of the final paranthesis (right now it will be func(int a,int b,int) c
			int parenthInd = signature.lastIndexOf(")");
			signature.remove(parenthInd,1);
			signature.append(")");

			returnVal.append(QString("<dd>%1</dd>").arg(signature));
		}
	}
	if(!returnVal.isEmpty())
	{
		//There were scripting elements.  Finish of the returnVal.
		//Start the returnVal with this scriptables name.
		returnVal.prepend(QString("<dl><dt>%1</dt>").arg(getName()));
		//Close the definition list.
		returnVal.append("</dl>");
	}
	
	return returnVal;
}

QString Scriptable::getInfo()
{
	QString returnVal = UIEnabled::getInfo();
	QString scriptingInfo;
	scriptingInfo.append(getScriptingInfo());
	if(!scriptingInfo.isEmpty())
	{
		returnVal.append("<h4>Scripting Info</h4>");
		returnVal.append(scriptingInfo);
	}
	return returnVal;
}
void Scriptable::postDeserialize()
{
	propertyContainer_->getProperty("UIOrder")->setVisible(false);
	// All property values are stored in an init property container.  This container
	// is used to reset properties to their initial values whenever an state machine 
	// element is entered.
	//initPropertyContainer_->clear();
	//initPropertyContainer_->copyProperties(propertyContainer_);

	//foreach(QVector<QSharedPointer<Property>> propVec,initPropertyContainer_->getProperties())
	//{
	//	foreach(QSharedPointer<Property> prop,propVec)
	//	{	//We need to add these properties to the DesignConfig so that they'll appear
	//		//in the database file.  Since they're not serialized though, we always want to add them
	//		//at the end of the list of ids so that nothing else's id is pushed back by them. 
	//		//addManagedAsset() takes care of this for us.  Unserialized assets are always
	//		//added to DesignConfig's lists last.
	//		if(getDesignConfig())
	//			getDesignConfig()->addManagedAsset(prop);
	//	}
	//}
	UIEnabled::postDeserialize();
}

bool Scriptable::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	//Since a lot of things is scriptable now, this is an unreasonable requirement.  We're taking it out for now
	//and just not adding anything with no name or whitespace in its name to the scripting system.
	//if(getName().contains(" "))
	//{
	//	addError("Scriptable", "The name of a scriptable element cannot contain whitespace");
	//	return false;
	//}
	if(hasContentsForScriptEngine())
	{
		if(getName().contains(" "))
		{
			addError("The name of an element that has script properties or functions cannot contain whitespace.");
			return false;
		}
		if(getName().isEmpty())
		{
			addError("The name of an element that has script properties or functions cannot be empty.");
			return false;
		}
		if(!getName().contains(QRegularExpression("^[$A-Z_a-z][0-9A-Z_$a-z]*$")))
		{
			addError("The name of an element that has script properties or functions cannot start with a number and may only contain letters, numbers, '_' and '$'.");
			return false;
		}
	}
	return true;
}

/* \brief Relads the property container with the values backed up in backupProperties()
 *
 *	This basically iterates through the backed up properties and resets the values
 *	in the property container.
 */
void Scriptable::restoreProperties()
{
	propertyContainer_->setPropertiesToInitValues();
	//QStringList properties = initPropertyContainer_->getPropertyList();

	//foreach(QString propName,properties)
	//{
	//	propertyContainer_->setPropertyValue(propName,initPropertyContainer_->getPropertyValue(propName));
	//}
	//QMap<QString, QVariant>::iterator propItr = initialProperties_.begin();
	//while(propItr != initialProperties_.end())
	//{
	//	//There should never be a property in initialProperties_ that doesn't
	//	//exist in the property container
	//	Q_ASSERT(properties.contains(propItr.key()));
	//	propertyContainer_->setPropertyValue(propItr.key(),propItr.value());
	//	propItr++;
	//}
}

}; //namespace Picto
