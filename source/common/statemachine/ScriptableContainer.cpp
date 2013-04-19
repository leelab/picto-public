#include <QMap>
#include <QPair>
#include <QDebug>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QAction>
#include <QTextEdit>
#include <QCoreApplication>

#include "ScriptableContainer.h"
#include "../property/EnumProperty.h"

#include "../parameter/AnalysisElement.h"

#include "../parameter/BooleanParameter.h"
#include "../parameter/ChoiceParameter.h"
#include "../parameter/NumericParameter.h"
#include "../parameter/RangeParameter.h"
#include "../parameter/RandomIntParameter.h"
#include "../parameter/RandomDoubleParameter.h"
#include "../parameter/DoubleParameter.h"
#include "../parameter/PseudorandomIntParameter.h"
#include "../parameter/TimerParameter.h"
#include "../parameter/OperatorClickParameter.h"
#include "../parameter/SignalValueParameter.h"

#include "../stimuli/ArrowGraphic.h"
#include "../stimuli/BoxGraphic.h"
#include "../stimuli/DiamondGraphic.h"
#include "../stimuli/ShapeShifterGraphic.h"
#include "../stimuli/TokenTrayGraphic.h"
#include "../stimuli/CircleGraphic.h"
#include "../stimuli/EllipseGraphic.h"
#include "../stimuli/LineGraphic.h"
#include "../stimuli/GridGraphic.h"
#include "../stimuli/PictureGraphic.h"
#include "../stimuli/RandomlyFilledGridGraphic.h"
#include "../stimuli/TextGraphic.h"
#include "../stimuli/OperatorInfoGraphic.h"
#include "../stimuli/DigitalOutput.h"
#include "../stimuli/BinaryDataOutput.h"
#include "../stimuli/AudioElement.h"
#include "../controlelements/circletarget.h"
#include "../controlelements/recttarget.h"
#include "../statemachine/ScriptFunction.h"
#include "../memleakdetect.h"

using namespace Picto;

ScriptableContainer* ScriptableContainer::objectForResultCheck_ = NULL;
QScriptValue ScriptableContainer::checkScriptResultScript_;

ScriptableContainer::ScriptableContainer()
:	parameterFactory_(new AssetFactory(0,-1)),
	visualElementFactory_(new AssetFactory(0,-1)),
	controlTargetFactory_(new AssetFactory(0,-1)),
	audioElementFactory_(new AssetFactory(0,-1)),
	outputSignalFactory_(new AssetFactory(0,-1)),
	scriptFunctionFactory_(new AssetFactory(0,-1)),
	scriptingInitialized_(false),
	debuggingEnabled_(false)
{
	isAnalysisElement_ = dynamic_cast<AnalysisElement*>(this);
	AddDefinableObjectFactory("ScriptFunction",scriptFunctionFactory_);
	scriptFunctionFactory_->addAssetType("ScriptFunction",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ScriptFunction::Create))));

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
	parameterFactory_->addAssetType("Double",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(DoubleParameter::Create))));
	parameterFactory_->addAssetType("PseudorandomInt",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PseudorandomIntParameter::Create))));
	parameterFactory_->addAssetType("Timer",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TimerParameter::Create))));
	parameterFactory_->addAssetType("OperatorClick",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(OperatorClickParameter::Create))));
	parameterFactory_->addAssetType("SignalValue",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SignalValueParameter::Create))));


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
	visualElementFactory_->addAssetType(TokenTrayGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TokenTrayGraphic::Create))));
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

	AddDefinableObjectFactory("OutputSignal",outputSignalFactory_);
	outputSignalFactory_->addAssetType("DigitalOutput",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(DigitalOutput::Create))));

	AddDefinableObjectFactory("OutputSignal",outputSignalFactory_);
	outputSignalFactory_->addAssetType("BinaryDataOutput",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(BinaryDataOutput::Create))));


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
	QSharedPointer<AnalysisElement> anaElement = strScriptable.dynamicCast<AnalysisElement>();
	if(anaElement)
	{
		//The input scriptable is part of an Analysis.  Only add it if its part of the same analysis
		//as me
		if(isAnalysisElement_)
		{
			QUuid scriptableAnaId = anaElement->getAnalysisId();
			AnalysisElement* thisAsAnaElem = dynamic_cast<AnalysisElement*>(this);
			if(thisAsAnaElem->getAnalysisId() != scriptableAnaId)
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
	if(canHaveScripts() && hasScripts() )
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

		QMap<QString,QPair<QString,QString>>  scriptMap = getScripts();
		for(QMap<QString,QPair<QString,QString>>::iterator it = scriptMap.begin();it!=scriptMap.end();it++)
		{
			if(qsEngine_->globalObject().property(it.key()).isValid())
			{
				// There is already a scriptable component with this name.
				Q_ASSERT(false);
				return false;
			}

			//Make a Qt Script Function out of the script and its name
			QString scriptTag = it.value().second;
			QString script = propertyContainer_->getPropertyValue(scriptTag).toString();
			Q_ASSERT(script.length());
			QString function = "function " + it.key() + "("+it.value().first+") { " + script + "\n}";

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

			//Connect this property's edited slot to deinitScripting, because if the script changes, we'll need to reinsert
			//it into the scripting environment
			disconnect(propertyContainer_->getProperty(scriptTag).data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(deinitScripting(Property*,QVariant)));
			connect(propertyContainer_->getProperty(scriptTag).data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(deinitScripting(Property*,QVariant)));
		}
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

void ScriptableContainer::ClearAnalysisChildren(QUuid analysisId)
{
	Scriptable::ClearAnalysisChildren(analysisId);

	//Go through the scriptables_ list and remove all analysis children with the input analysis id.
	bool somethingWasRemoved = false;
	AnalysisElement* analysisElem;
	for(QList<QWeakPointer<Scriptable>>::iterator iter = scriptables_.begin();iter!=scriptables_.end();)
	{
		Q_ASSERT(!iter->isNull());
		analysisElem = dynamic_cast<AnalysisElement*>(iter->data());
		if(!analysisElem)
		{
			iter++;
			continue;
		}
		if(analysisElem->getAnalysisId() == analysisId)
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


bool ScriptableContainer::searchForQuery(SearchRequest searchRequest)
{
	if(Scriptable::searchForQuery(searchRequest))
		return true;
	switch(searchRequest.type)
	{
	case SearchRequest::SCRIPT:
		{
			//Check to see if this object contains any non-empty scripts
			QMap<QString,QPair<QString,QString>>  scriptMap = getScripts();
			QSharedPointer<Property> currScriptProp;
			QString currScript;
			foreach(QString key,scriptMap.keys())
			{
				QPair<QString,QString> inputsNamePair = scriptMap.value(key);
				currScriptProp = propertyContainer_->getProperty(inputsNamePair.second);
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
			QMap<QString,QPair<QString,QString>>  scriptMap = getScripts();
			QSharedPointer<Property> currScriptProp;
			QString currScript;
			foreach(QString key,scriptMap.keys())
			{
				QPair<QString,QString> inputsNamePair = scriptMap.value(key);
				//Search script inputs
				if(inputsNamePair.first.contains(searchRequest.query,searchRequest.caseSensitive?Qt::CaseSensitive:Qt::CaseInsensitive))
					return true;

				//Search script contents
				currScriptProp = propertyContainer_->getProperty(inputsNamePair.second);
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
	QString scriptToCall;
	if(debuggingEnabled_)
	{
		scriptToCall = QString("var returnVal = %1();\n%2(\"%3\",returnVal);\nreturnVal;").arg(scriptName).arg("checkScriptResults").arg(scriptName);
	}
	else
		scriptToCall = QString("%1();").arg(scriptName);
	scriptReturnVal = qsEngine_->evaluate(scriptToCall);
	objectForResultCheck_ = NULL;
}

QString ScriptableContainer::getReturnValueError(QString,const QScriptValue&)
{
	return QString();
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
			addChildToScriptLists(tagChild);
		}
	}
	connect(this,SIGNAL(childAddedAfterDeserialize(QSharedPointer<Asset>)),this,SLOT(addChildToScriptLists(QSharedPointer<Asset>)));
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
	//			addError("ScriptableContainer", QString("Muliple scriptables have the same name: \"%1\"").arg((*ita)->getName()).toLatin1(), xmlStreamReader);
	//			return false;
	//		}
	//	}
	//}
	return true;
}

bool ScriptableContainer::bindScriptablesToScriptEngine(QScriptEngine &engine)
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