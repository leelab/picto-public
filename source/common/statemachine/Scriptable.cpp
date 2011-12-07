#include <QDebug>
#include <QMetaProperty>
#include "Scriptable.h"
#include "../memleakdetect.h"
namespace Picto {

Scriptable::Scriptable()
{
	initPropertyContainer_ = PropertyContainer::create("InitProperties");
	AddDefinableProperty(QVariant::Bool,"UIEnabled",false);
	AddDefinableProperty(QVariant::Int,"UIOrder",0);
}

bool Scriptable::bindToScriptEngine(QScriptEngine &engine)
{
	//Add object to scripting environment.

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
			return true;
	}

	//If we got here, there's something to add to the script engine.  Add it.
	QScriptValue qsValue = engine.newQObject(this,QScriptEngine::QtOwnership);
	engine.globalObject().setProperty(getName(),qsValue);
	//Check for errors
	if(engine.hasUncaughtException())
	{
		QString errorMsg = "Uncaught exception in Script Element " + getName() +"\n";
		errorMsg += QString("Line %1: %2\n").arg(engine.uncaughtExceptionLineNumber())
										  .arg(engine.uncaughtException().toString());
		errorMsg += QString("Backtrace: %1\n").arg(engine.uncaughtExceptionBacktrace().join(", "));
		qDebug()<<errorMsg;
		return false;
	}

	////Add its Properties as scriptable as well.
	//QStringList props = propertyContainer_->getPropertyList();
	//foreach(QString propStr,props)
	//{
	//	QSharedPointer<Property> prop = propertyContainer_->getProperty(propStr);
	//	Q_ASSERT(prop);
	//	if(!prop->isScriptEditable())
	//		continue;
	//	QScriptValue childValue = engine.newQObject(prop.data(),QScriptEngine::QtOwnership,QScriptEngine::ExcludeSuperClassContents);
	//	qsValue.setProperty(propStr,childValue);
	//	//Check for errors
	//	if(engine.hasUncaughtException())
	//	{
	//		QString errorMsg = "Uncaught exception in Script Element " + getName() +"\n";
	//		errorMsg += QString("Line %1: %2\n").arg(engine.uncaughtExceptionLineNumber())
	//										  .arg(engine.uncaughtException().toString());
	//		errorMsg += QString("Backtrace: %1\n").arg(engine.uncaughtExceptionBacktrace().join(", "));
	//		qDebug()<<errorMsg;
	//		return false;
	//	}
	//}

	return true;
}

/*!	\brief Resets the VisualElement
 *
 *	Since Scriptables can be modified by scripts, resetting them returns them
 *	to their initial state.  This basically just reloads the PropertyContainer
 *	with the initial properties
 */
void Scriptable::reset()
{
	restoreProperties();
}

void Scriptable::setPropertyRuntimeEditable(QString propName, bool editable)
{
	initPropertyContainer_->getProperty(propName)->setRuntimeEditable(editable);
}

QList<QSharedPointer<Property>> Scriptable::getDescendantsProperties()
{
	QList<QSharedPointer<Property>> descendantProps;
	QHash<QString, QVector<QSharedPointer<Property>>> propMap = initPropertyContainer_->getProperties();
	foreach(QVector<QSharedPointer<Property>> propVec, propMap)
	{
		foreach(QSharedPointer<Property> prop, propVec)
		{
			descendantProps.append(prop);
		}
	}
	descendantProps.append(UIEnabled::getDescendantsProperties());
	return descendantProps;
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
			QString signature = metaMeth.signature();
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
	// All property values are stored in an init property container.  This container
	// is used to reset properties to their initial values whenever an state machine 
	// element is entered.
	initPropertyContainer_->clear();
	initPropertyContainer_->copyProperties(propertyContainer_);
	foreach(QVector<QSharedPointer<Property>> propVec,initPropertyContainer_->getProperties())
	{
		foreach(QSharedPointer<Property> prop,propVec)
		{	//We need to add these properties to the experimentConfig so that they'll appear
			//in the database file.  Since they're not serialized though, we always want to add them
			//at the end of the list of ids so that nothing else's id is pushed back by them. 
			//addManagedAsset() takes care of this for us.  Unserialized assets are always
			//added to experimentConfig's lists last.
			if(getExperimentConfig())
				getExperimentConfig()->addManagedAsset(prop);
		}
	}
	UIEnabled::postDeserialize();
}

bool Scriptable::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	if(getName().contains(" "))
	{
		addError("Scriptable", "The name of a scriptable element cannot contain whitespace",xmlStreamReader);
		return false;
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
	QStringList properties = initPropertyContainer_->getPropertyList();

	foreach(QString propName,properties)
	{
		propertyContainer_->setPropertyValue(propName,initPropertyContainer_->getPropertyValue(propName));
	}
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
