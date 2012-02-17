#include <QMap>
#include <QDebug>
#include <QScriptValueIterator>

#include "ScriptableContainer.h"
#include "../property/EnumProperty.h"

#include "../parameter/BooleanParameter.h"
#include "../parameter/ChoiceParameter.h"
#include "../parameter/NumericParameter.h"
#include "../parameter/RangeParameter.h"
#include "../parameter/RandomIntParameter.h"
#include "../parameter/RandomDoubleParameter.h"
#include "../parameter/PseudorandomIntParameter.h"
#include "../parameter/TimerParameter.h"
#include "../parameter/OperatorClickParameter.h"

#include "../stimuli/ArrowGraphic.h"
#include "../stimuli/BoxGraphic.h"
#include "../stimuli/DiamondGraphic.h"
#include "../stimuli/ShapeShifterGraphic.h"
#include "../stimuli/CircleGraphic.h"
#include "../stimuli/EllipseGraphic.h"
#include "../stimuli/LineGraphic.h"
#include "../stimuli/GridGraphic.h"
#include "../stimuli/PictureGraphic.h"
#include "../stimuli/RandomlyFilledGridGraphic.h"
#include "../stimuli/TextGraphic.h"
#include "../stimuli/OperatorInfoGraphic.h"
#include "../stimuli/AudioElement.h"
#include "../controlelements/circletarget.h"
#include "../controlelements/recttarget.h"
#include "../memleakdetect.h"

using namespace Picto;

ScriptableContainer::ScriptableContainer()
:	parameterFactory_(new AssetFactory(0,-1)),
	visualElementFactory_(new AssetFactory(0,-1)),
	controlTargetFactory_(new AssetFactory(0,-1)),
	audioElementFactory_(new AssetFactory(0,-1)),
	scriptingInitialized_(false)
{
	AddDefinableObjectFactory("Parameter",parameterFactory_);
	parameterFactory_->addAssetType("Boolean",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(BooleanParameter::Create))));
	//parameterFactory_->addAssetType("Choice",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ChoiceParameter::Create))));
	parameterFactory_->addAssetType("Numeric",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(NumericParameter::Create))));
	parameterFactory_->addAssetType("Range",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RangeParameter::Create))));
	parameterFactory_->addAssetType("RandomInt",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RandomIntParameter::Create))));
	parameterFactory_->addAssetType("RandomDouble",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RandomDoubleParameter::Create))));
	parameterFactory_->addAssetType("PseudorandomInt",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PseudorandomIntParameter::Create))));
	parameterFactory_->addAssetType("Timer",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TimerParameter::Create))));
	parameterFactory_->addAssetType("OperatorClick",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(OperatorClickParameter::Create))));


	AddDefinableObjectFactory("VisualElement",visualElementFactory_);
	//For the sake of cleanliness, we should probably have a StimulusContainer class that adds all of the following.
	//functionally, there is no problem with just adding them here.
	visualElementFactory_->addAssetType(ArrowGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ArrowGraphic::Create))));
	visualElementFactory_->addAssetType(BoxGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(BoxGraphic::Create))));
	visualElementFactory_->addAssetType(DiamondGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(DiamondGraphic::Create))));
	visualElementFactory_->addAssetType(CircleGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CircleGraphic::Create))));
	visualElementFactory_->addAssetType(EllipseGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(EllipseGraphic::Create))));
	visualElementFactory_->addAssetType(ShapeShifterGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ShapeShifterGraphic::Create))));
	visualElementFactory_->addAssetType(LineGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LineGraphic::Create))));
	visualElementFactory_->addAssetType(GridGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(GridGraphic::Create))));
	visualElementFactory_->addAssetType(PictureGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PictureGraphic::Create))));
	visualElementFactory_->addAssetType(RandomlyFilledGridGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RandomlyFilledGridGraphic::Create))));
	visualElementFactory_->addAssetType(TextGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TextGraphic::Create))));
	visualElementFactory_->addAssetType(OperatorInfoGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(OperatorInfoGraphic::Create))));


	AddDefinableObjectFactory("ControlTarget",controlTargetFactory_);
	controlTargetFactory_->addAssetType("CircleTarget",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CircleTarget::Create))));
	controlTargetFactory_->addAssetType("RectTarget",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RectTarget::Create))));

	AddDefinableObjectFactory("AudioElement",audioElementFactory_);
	audioElementFactory_->addAssetType("AudioElement",
		QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(AudioElement::Create))));


}

/*! \brief Copies the scriptables from the passed in container
 *	
 *	This is used when adding an child element to a parent scriptableContainer.  By passing
 *	in the parent scriptable container, we now have access to all 
 *	of the parent's scriptables.  Effectively this allows us to
 *	have scriptables with global scope.
 */
void ScriptableContainer::addScriptables(QSharedPointer<ScriptableContainer> scriptableContainer)
{
	addScriptables(scriptableContainer.data());
}

void ScriptableContainer::addScriptables(ScriptableContainer *scriptableContainer)
{
	QList<QWeakPointer<Scriptable>> scriptables = scriptableContainer->getScriptableList();
	foreach(QWeakPointer<Scriptable> scriptable,scriptables)
	{
		addScriptable(scriptable);
	}
}

void ScriptableContainer::addScriptable(QWeakPointer<Scriptable> scriptable)
{
	if(scriptable.isNull())
		return;
	scriptables_.push_back(scriptable);
	//If we added a new scriptable, scripting is no longer properly initialized.
	scriptingInitialized_ = false;
	//If the new scriptable's name edited, we'll need to reinitialize scripting again.
	connect(scriptable.toStrongRef().data(),SIGNAL(nameEdited()),this,SLOT(deinitScripting()));
	foreach(QSharedPointer<ScriptableContainer> child,scriptableContainers_)
	{
		child->addScriptable(scriptable);
	}
}

void ScriptableContainer::addChildScriptableContainer(QSharedPointer<ScriptableContainer> child)
{
	scriptableContainers_.push_back(child);
	child->addScriptables(this);
	if(scriptingInitialized_)
	{	//Our engine and all child ScriptableContainers engines are initialized, so 
		//initialize this one too.
		child->initScripting();
	}
}

//! \brief Returns a list of all contained parameters
QList<QWeakPointer<Scriptable>> ScriptableContainer::getScriptableList()
{
	return scriptables_;
}

bool ScriptableContainer::initScripting()
{
	if(	scriptingInitialized_ )
		return true;

	//Initialize scripting on this ScriptableContainer
	if(canHaveScripts() && hasScripts() )
	{
		//Looks like we're going to need a script engine
		//create the engine
		qsEngine_ = QSharedPointer<QScriptEngine>(new QScriptEngine());

		//Bind all scriptable to the script engine (parameters and ui elements)
		if(!bindToScriptEngine(*qsEngine_))
			return false;

		QMap<QString,QString> scriptMap = getScripts();
		for(QMap<QString,QString>::iterator it = scriptMap.begin();it!=scriptMap.end();it++)
		{
			if(qsEngine_->globalObject().property(it.key()).isValid())
			{
				// There is already a scriptable component with this name.
				return false;
			}

			//Make a Qt Script Function out of the script and its name
			QString function = "function " + it.key() + "() { " + it.value() + "}";

			//add the function to the engine by calling evaluate on it
			qsEngine_->evaluate(function);
			//Check for errors
			if(qsEngine_->hasUncaughtException())
			{
				QString errorMsg = "Uncaught exception in Script Element " + getName() +"\n";
				errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
												  .arg(qsEngine_->uncaughtException().toString());
				errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
				qDebug()<<errorMsg;
				return false;
			}
		}
	}
	
	//Inform child classes that the scriptable container has been reinitialized.
	scriptableContainerWasReinitialized();

	//Intialize Scripting on all child ScriptableContainers
	foreach(QSharedPointer<ScriptableContainer> child,scriptableContainers_)
	{
		child->initScripting();
	}
	scriptingInitialized_ = true;
	return true;
}

void ScriptableContainer::resetScriptableValues()
{

	QStringList childTags = getDefinedChildTags();
	foreach(QString childTag,childTags)
	{
		QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> tagChild,tagChildren)
		{
			if(tagChild->inherits("Picto::Scriptable"))
			{
				tagChild.staticCast<Scriptable>()->reset();
			}
		}
	}
}

QString ScriptableContainer::getInfo()
{
	QString returnVal = Scriptable::getInfo();
	QString scriptMemberStr;
	if(canHaveScripts())
	{
		foreach(QSharedPointer<Scriptable> scriptable, scriptables_)
		{
			scriptMemberStr.append(scriptable->getScriptingInfo());
		}
		if(returnVal.length())
		{
			scriptMemberStr.prepend("<h4>Available Scriptable Members</h4>");
		}
		else
			scriptMemberStr.append("<h4>No Scriptable Members Available</h4>");
		returnVal.append(scriptMemberStr);
	}
	return returnVal;
}

void ScriptableContainer::runScript(QString scriptName)
{
	bool returnVal;
	runScript(scriptName,returnVal);
}

void ScriptableContainer::runScript(QString scriptName, bool& scriptReturnVal)
{
	scriptReturnVal = qsEngine_->globalObject().property(scriptName).call().toBool();
	if(qsEngine_->hasUncaughtException())
	{
		QString errorMsg = "Uncaught exception in State" + getName() +", script "+scriptName+"\n";
		errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
										  .arg(qsEngine_->uncaughtException().toString());
		errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
		qDebug()<<errorMsg;
	}
}

void ScriptableContainer::postDeserialize()
{
	Scriptable::postDeserialize();
	QStringList childTags = getDefinedChildTags();
	foreach(QString childTag,childTags)
	{
		QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> tagChild,tagChildren)
		{
			if(tagChild->inherits("Picto::Scriptable"))
			{
				addScriptable(tagChild.staticCast<Scriptable>());
			}
		}
	}
}

bool ScriptableContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Scriptable::validateObject(xmlStreamReader))
		return false;
	
	//We were checking whether multiple scriptables had the same name below.  This wasn't a complete
	//check though because we didn't check scriptable names from our children against scriptable names
	//that are also in scope from above us.  In any event, when we implement script validation, we should
	//get an error when the second scriptable attempts to add itself to the script engine.

	//QList<QSharedPointer<Asset>> newScriptables = getGeneratedChildren("Scriptable");
	//QList<QSharedPointer<Asset>>::iterator ita,itb;
	//for(ita = newScriptables.begin();ita != newScriptables.end();ita++)
	//{
	//	for(itb = ita+1;itb != newScriptables.end();itb++)
	//	{
	//		if((*itb)->getName() == (*ita)->getName())
	//		{
	//			addError("ScriptableContainer", QString("Muliple scriptables have the same name: \"%1\"").arg((*ita)->getName()).toAscii(), xmlStreamReader);
	//			return false;
	//		}
	//	}
	//}
	return true;
}

bool ScriptableContainer::bindToScriptEngine(QScriptEngine &engine)
{
	foreach(QSharedPointer<Scriptable> scriptable, scriptables_)
	{
		if(!scriptable->bindToScriptEngine(engine))
			return false;
	}
	return true;
}

void ScriptableContainer::deinitScripting()
{
	scriptingInitialized_ = false;
}
