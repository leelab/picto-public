#include <QMap>
#include <QPair>
#include <QDebug>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QAction>
#include <QTextEdit>
#include <QCoreApplication>
#include <QScriptSyntaxCheckResult>

#include "ScriptableContainer.h"
#include "../property/EnumProperty.h"

#include "../parameter/AssociateElement.h"
//
//#include "../parameter/BooleanParameter.h"
//#include "../parameter/ChoiceParameter.h"
//#include "../parameter/NumericParameter.h"
//#include "../parameter/RangeParameter.h"
//#include "../parameter/RandomIntParameter.h"
//#include "../parameter/RandomDoubleParameter.h"
//#include "../parameter/DoubleParameter.h"
//#include "../parameter/PseudorandomIntParameter.h"
//#include "../parameter/TimerParameter.h"
//#include "../parameter/OperatorClickParameter.h"
//#include "../parameter/SignalValueParameter.h"
//
//#include "../stimuli/ArrowGraphic.h"
//#include "../stimuli/BoxGraphic.h"
//#include "../stimuli/DiamondGraphic.h"
//#include "../stimuli/ShapeShifterGraphic.h"
//#include "../stimuli/TokenTrayGraphic.h"
//#include "../stimuli/CircleGraphic.h"
//#include "../stimuli/EllipseGraphic.h"
//#include "../stimuli/LineGraphic.h"
//#include "../stimuli/GridGraphic.h"
//#include "../stimuli/PictureGraphic.h"
//#include "../stimuli/RandomlyFilledGridGraphic.h"
//#include "../stimuli/TextGraphic.h"
//#include "../stimuli/OperatorInfoGraphic.h"
//#include "../stimuli/DigitalOutput.h"
//#include "../stimuli/BinaryDataOutput.h"
//#include "../stimuli/AudioElement.h"
//#include "../controlelements/circletarget.h"
//#include "../controlelements/recttarget.h"
//#include "../statemachine/ScriptFunction.h"
#include "../memleakdetect.h"

using namespace Picto;

ScriptableContainer* ScriptableContainer::objectForResultCheck_ = NULL;
QScriptValue ScriptableContainer::checkScriptResultScript_;

ScriptableContainer::ScriptableContainer()
:	/*parameterFactory_(new AssetFactory(0,-1)),
	visualElementFactory_(new AssetFactory(0,-1)),
	controlTargetFactory_(new AssetFactory(0,-1)),
	audioElementFactory_(new AssetFactory(0,-1)),
	outputSignalFactory_(new AssetFactory(0,-1)),
	scriptFunctionFactory_(new AssetFactory(0,-1)),*/
	scriptingInitialized_(false),
	debuggingEnabled_(false)
{
	isAssociateElement_ = dynamic_cast<AssociateElement*>(this);
	//AddDefinableObjectFactory("ScriptFunction",scriptFunctionFactory_);
	//scriptFunctionFactory_->addAssetType("ScriptFunction",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ScriptFunction::Create))));

	//AddDefinableObjectFactory("Parameter",parameterFactory_);
	//parameterFactory_->addAssetType("Boolean",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(BooleanParameter::Create))));
	////parameterFactory_->addAssetType("Choice",
	////	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ChoiceParameter::Create))));
	//parameterFactory_->addAssetType("Numeric",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(NumericParameter::Create))));
	//parameterFactory_->addAssetType("Range",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RangeParameter::Create))));
	//parameterFactory_->addAssetType("RandomInt",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RandomIntParameter::Create))));
	//parameterFactory_->addAssetType("RandomDouble",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RandomDoubleParameter::Create))));
	//parameterFactory_->addAssetType("Double",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(DoubleParameter::Create))));
	//parameterFactory_->addAssetType("PseudorandomInt",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PseudorandomIntParameter::Create))));
	//parameterFactory_->addAssetType("Timer",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TimerParameter::Create))));
	//parameterFactory_->addAssetType("OperatorClick",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(OperatorClickParameter::Create))));
	//parameterFactory_->addAssetType("SignalValue",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SignalValueParameter::Create))));


	//AddDefinableObjectFactory("VisualElement",visualElementFactory_);
	////For the sake of cleanliness, we should probably have a StimulusContainer class that adds all of the following.
	////functionally, there is no problem with just adding them here.
	//visualElementFactory_->addAssetType(ArrowGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ArrowGraphic::Create))));
	//visualElementFactory_->addAssetType(BoxGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(BoxGraphic::Create))));
	//visualElementFactory_->addAssetType(DiamondGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(DiamondGraphic::Create))));
	//visualElementFactory_->addAssetType(CircleGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CircleGraphic::Create))));
	//visualElementFactory_->addAssetType(EllipseGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(EllipseGraphic::Create))));
	//visualElementFactory_->addAssetType(ShapeShifterGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ShapeShifterGraphic::Create))));
	//visualElementFactory_->addAssetType(TokenTrayGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TokenTrayGraphic::Create))));
	//visualElementFactory_->addAssetType(LineGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LineGraphic::Create))));
	//visualElementFactory_->addAssetType(GridGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(GridGraphic::Create))));
	//visualElementFactory_->addAssetType(PictureGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PictureGraphic::Create))));
	//visualElementFactory_->addAssetType(RandomlyFilledGridGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RandomlyFilledGridGraphic::Create))));
	//visualElementFactory_->addAssetType(TextGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TextGraphic::Create))));
	//visualElementFactory_->addAssetType(OperatorInfoGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(OperatorInfoGraphic::Create))));

	//AddDefinableObjectFactory("ControlTarget",controlTargetFactory_);
	//controlTargetFactory_->addAssetType("CircleTarget",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CircleTarget::Create))));
	//controlTargetFactory_->addAssetType("RectTarget",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RectTarget::Create))));

	//AddDefinableObjectFactory("AudioElement",audioElementFactory_);
	//audioElementFactory_->addAssetType("AudioElement",
	//	QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(AudioElement::Create))));

	//AddDefinableObjectFactory("OutputSignal",outputSignalFactory_);
	//outputSignalFactory_->addAssetType("DigitalOutput",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(DigitalOutput::Create))));

	//AddDefinableObjectFactory("OutputSignal",outputSignalFactory_);
	//outputSignalFactory_->addAssetType("BinaryDataOutput",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(BinaryDataOutput::Create))));


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
	//If the scriptable is empty (ie. Whatever the weak pointer was pointing to has been deleted) return
	if(scriptable.isNull())
		return;
	QSharedPointer<Scriptable> strScriptable = scriptable.toStrongRef();
	QSharedPointer<AssociateElement> assocElement = strScriptable.dynamicCast<AssociateElement>();
	if(assocElement)
	{
		//The input scriptable is part of an Associate.  Only add it if its part of the same Associate
		//as me
		if(isAssociateElement_)
		{
			QUuid scriptableAssocId = assocElement->getAssociateId();
			AssociateElement* thisAsAssocElem = dynamic_cast<AssociateElement*>(this);
			if(thisAsAssocElem->getAssociateId() != scriptableAssocId)
				return;
			scriptables_.push_back(scriptable);
		}
	}
	else
		scriptables_.push_back(scriptable);

	//If we added a new scriptable, scripting is no longer properly initialized.
	scriptingInitialized_ = false;
	//If the new scriptable's name was edited, we'll need to reinitialize scripting again.
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
		child->initScripting(debuggingEnabled_);
	}
}

//! \brief Returns a list of all contained parameters
QList<QWeakPointer<Scriptable>> ScriptableContainer::getScriptableList()
{
	return scriptables_;
}

bool ScriptableContainer::initScripting(bool enableDebugging)
{
	if(	scriptingInitialized_ )
		return true;
	
	debuggingEnabled_ = enableDebugging;
	//Initialize scripting on this ScriptableContainer
	if(canHaveScripts())
	{
		if(hasScripts())
		{
			//Looks like we're going to need a script engine
			//create the engine
			qsEngine_ = QSharedPointer<QScriptEngine>(new QScriptEngine());

			if(debuggingEnabled_)
			{
				//Attach the debugger to the script engine
				qsEngineDebugger_ = QSharedPointer<QScriptEngineDebugger>(new QScriptEngineDebugger());
				qsEngineDebugger_->attachTo(qsEngine_.data());

				//Add result check function to qsEngine
				qsEngine_->globalObject().setProperty("checkScriptResults",qsEngine_->newFunction(checkScriptResults));
			}


			//Bind all scriptable to the script engine (parameters and ui elements)
			if(!bindScriptablesToScriptEngine(*qsEngine_))
				return false;

			QMap<QString,QString>  scriptMap = getScripts();
			for(QMap<QString,QString>::iterator it = scriptMap.begin();it!=scriptMap.end();it++)
			{
				//if(qsEngine_->globalObject().property(it.key()).isValid())
				//{
				//	// There is already a scriptable component with this name.
				//	Q_ASSERT(false);
				//	return false;
				//}

				//Make a Qt Script Function out of the script and its name
				QString scriptTag = it.value();
				QString script = propertyContainer_->getPropertyValue(scriptTag).toString();
				Q_ASSERT(script.length());
				Q_ASSERT(!scriptPrograms_.contains(it.key()));
				QString programText;
				if(!debuggingEnabled_)
				{
					//Add a version of the script with return value checking.
					programText = QString("function script(){\n\n//YOUR CODE STARTS HERE-------\n%1\n//YOUR CODE ENDS HERE----------\n\n}\nscript()").arg(script);
				}
				else
				{
					programText = QString("function script(){\n\n//YOUR CODE STARTS HERE-------\n%1\n//YOUR CODE ENDS HERE----------\n\n}\nvar returnVal = script();\ncheckScriptResults(\"%2\",returnVal);\nreturnVal;").arg(script).arg(it.key());
				}
				scriptPrograms_[it.key()] = QSharedPointer<QScriptProgram>(new QScriptProgram(programText));
			}
		}
		//Connect this object's edited slot to deinitScripting, because if one of its scripts changes, we'll need to reinsert
		//it into the scripting environment
		disconnect(this,SIGNAL(edited()),this,SLOT(deinitScripting()));
		connect(this,SIGNAL(edited()),this,SLOT(deinitScripting()));
	}
	
	//Inform child classes that the scriptable container has been reinitialized.
	scriptableContainerWasReinitialized();

	//Intialize Scripting on all child ScriptableContainers
	foreach(QSharedPointer<ScriptableContainer> child,scriptableContainers_)
	{
		child->initScripting(debuggingEnabled_);
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

void ScriptableContainer::ClearAssociateChildren(QUuid associateId)
{
	Scriptable::ClearAssociateChildren(associateId);

	//Go through the scriptables_ list and remove all associate children with the input associate id.
	bool somethingWasRemoved = false;
	AssociateElement* associateElem;
	for(QList<QWeakPointer<Scriptable>>::iterator iter = scriptables_.begin();iter!=scriptables_.end();)
	{
		Q_ASSERT(!iter->isNull());
		associateElem = dynamic_cast<AssociateElement*>(iter->data());
		if(!associateElem)
		{
			iter++;
			continue;
		}
		if(associateElem->getAssociateId() == associateId)
		{
			iter = scriptables_.erase(iter);
			somethingWasRemoved = true;
			continue;
		}
		iter++;
	}

	if(somethingWasRemoved)
	{
		//If we removed a scriptable, scripting is no longer properly initialized.
		scriptingInitialized_ = false;
	}
}

void ScriptableContainer::runScript(QString scriptName)
{
	QScriptValue returnVal;
	runScript(scriptName,returnVal);
}

void ScriptableContainer::runScript(QString scriptName, QScriptValue& scriptReturnVal)
{
	initScripting(debuggingEnabled_);	//Make sure this scriptable container has scripting initialized before attempting to run one of its scripts
	Q_ASSERT(qsEngine_);
	objectForResultCheck_ = this;
	Q_ASSERT(scriptPrograms_.contains(scriptName));
	scriptReturnVal = qsEngine_->evaluate(*scriptPrograms_.value(scriptName));
	objectForResultCheck_ = NULL;
}

QString ScriptableContainer::getReturnValueError(QString,const QScriptValue&)
{
	return QString();
}

void ScriptableContainer::postDeserialize()
{
	Scriptable::postDeserialize();
	scriptPrograms_.clear();
	QStringList childTags = getDefinedChildTags();
	foreach(QString childTag,childTags)
	{
		QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> tagChild,tagChildren)
		{
			addChildToScriptLists(tagChild);
		}
	}
	connect(this,SIGNAL(childAddedAfterDeserialize(QSharedPointer<Asset>)),this,SLOT(addChildToScriptLists(QSharedPointer<Asset>)));
}

bool ScriptableContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Scriptable::validateObject(xmlStreamReader))
		return false;
	
	//Check Script Syntax
	//First try to initialize scripting.  Since we'll need useful feedback from this, DO IT LATER

	QMap<QString,QString> scriptMap = getScripts();
	bool scriptsAreOkay = true;
	for(QMap<QString,QString>::iterator it = scriptMap.begin();it!=scriptMap.end();it++)
	{
		QString scriptTag = it.value();
		QString script = propertyContainer_->getPropertyValue(scriptTag).toString();
		QString function = "function " + it.key() + "() { " + script + "}";
		QScriptSyntaxCheckResult res = QScriptEngine::checkSyntax(function);
		if(res.state() != QScriptSyntaxCheckResult::Valid)
		{
			QString errorMessage = QString("Syntax Error in \"%1\"(Line:%2)")
				.arg(scriptTag)
				.arg(res.errorLineNumber())
				.arg(res.errorMessage());
			if(res.errorMessage().size())
				errorMessage.append(" : " + res.errorMessage());
			addError(errorMessage);
			scriptsAreOkay = false;
		}
		if(!scriptsAreOkay)
			return false;
	}
	//We don't check if multiple scriptables have the same name, because or bindScriptablesToScriptEngine()
	//function makes sure that only the one with the longest path is added to the scriptEngine
	return true;
}

bool ScriptableContainer::executeSearchAlgorithm(SearchRequest searchRequest)
{
	if(Scriptable::executeSearchAlgorithm(searchRequest))
		return true;
	switch(searchRequest.type)
	{
	case SearchRequest::SCRIPT:
		{
			//Check to see if this object contains any non-empty scripts
			QMap<QString,QString>  scriptMap = getScripts();
			QSharedPointer<Property> currScriptProp;
			QString currScript;
			foreach(QString key,scriptMap.keys())
			{
				QString propName = scriptMap.value(key);
				currScriptProp = propertyContainer_->getProperty(propName);
				if(currScriptProp.isNull())
					continue;
				if(!currScriptProp->value().toString().isEmpty())
					return true;
			}
		}
		break;
	case SearchRequest::STRING:
		{
			//Search through my scripts to see if any contain the query string
			QMap<QString,QString>  scriptMap = getScripts();
			QSharedPointer<Property> currScriptProp;
			QString currScript;
			foreach(QString key,scriptMap.keys())
			{
				QString propName = scriptMap.value(key);
				//Search script contents
				currScriptProp = propertyContainer_->getProperty(propName);
				if(currScriptProp.isNull())
					continue;
				if(currScriptProp->value().toString().contains(searchRequest.query,searchRequest.caseSensitive?Qt::CaseSensitive:Qt::CaseInsensitive))
					return true;
			}
		}
		break;
	default:
		return false;
	};
	return false;
}

bool ScriptableContainer::bindScriptablesToScriptEngine(QScriptEngine &engine)
{
	//If any scriptables have the same name, the one with the shorter path
	//should not be bound.  Create a list with all bindable scriptables.
	QHash<QString,QSharedPointer<Scriptable>> scriptableHash;
	QString currName;
	foreach(QSharedPointer<Scriptable> scriptable, scriptables_)
	{
		currName = scriptable->getName();
		if(!scriptableHash.contains(currName))
		{
			scriptableHash[currName] = scriptable;
		}
		else
		{
			if(scriptableHash.value(currName)->getPath().size() < scriptable->getPath().size())
				scriptableHash[currName] = scriptable;
		}
	}
	QList<QSharedPointer<Scriptable>> bindableScriptables = scriptableHash.values();
	foreach(QSharedPointer<Scriptable> scriptable, bindableScriptables)
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

void ScriptableContainer::deinitScripting(Property*,QVariant)
{
	deinitScripting();
}

//If a child is added to this element after postDeserialize then we may need to add it
//using addScriptable() or addChildScriptableContainer().  We handle that here.
void ScriptableContainer::addChildToScriptLists(QSharedPointer<Asset> newChild)
{
	if(newChild.dynamicCast<Scriptable>())
		addScriptable(newChild.staticCast<Scriptable>());
	if(newChild.dynamicCast<ScriptableContainer>())
		addChildScriptableContainer(newChild.staticCast<ScriptableContainer>());
}

QScriptValue ScriptableContainer::checkScriptResults(QScriptContext *context, QScriptEngine*)
{
	QScriptValue returnVal;
	if(!objectForResultCheck_)
		return returnVal;
	if(context->argumentCount() != 2)
		return returnVal;
	QString resultError = objectForResultCheck_->getReturnValueError(context->argument(0).toString(),context->argument(1));
	if(resultError.isEmpty())
		return returnVal;
	context->throwError(resultError);
	return returnVal;
}