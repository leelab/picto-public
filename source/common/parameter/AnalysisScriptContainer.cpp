#include "AnalysisScriptContainer.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisScriptContainer::AnalysisScriptContainer()
: ScriptableContainer()
{
	EXP_LINK_FACTORY_CREATION
	//Depending whether this is attached to a state, a MachineContainer, a StateMachineElement, a result, or a logic result, there may not be any
	//need for AnalysisEntryScript, AnalysisFrameScript or AnalysisExitScript
	AddDefinableProperty(QVariant::String,"AnalysisEntryScript","",QMap<QString,QVariant>(),0,1);
	AddDefinableProperty(QVariant::String,"AnalysisFrameScript","",QMap<QString,QVariant>(),0,1);
	AddDefinableProperty(QVariant::String,"AnalysisExitScript","",QMap<QString,QVariant>(),0,1);
	requireUniqueName(false);
}

QSharedPointer<Asset> AnalysisScriptContainer::Create()
{
	return QSharedPointer<Asset>(new AnalysisScriptContainer());
}

void AnalysisScriptContainer::runScript(ScriptType type)
{
	//Monitor for changes in any Experimental properties during the running of the scripts below.
	//If any experimental properties change, Analysis changed them and a runtime error should be issued.
	Property::startMonitoringForExperimentalValueChange();
	switch(type)
	{
	case ENTRY:
		if(!propertyContainer_->getPropertyValue("AnalysisEntryScript").toString().isEmpty())
		{
			QString scriptNamePrefix = getScriptNamePrefix();
			ScriptableContainer::runScript(scriptNamePrefix+"Entry");
		}
		break;
	case FRAME:
		if(propertyContainer_->getProperty("AnalysisFrameScript") && (!propertyContainer_->getPropertyValue("AnalysisFrameScript").toString().isEmpty()))
		{
			QString scriptNamePrefix = getScriptNamePrefix();
			ScriptableContainer::runScript(scriptNamePrefix+"Frame");
		}
		break;
	case EXIT:
		if(propertyContainer_->getProperty("AnalysisExitScript") && (!propertyContainer_->getPropertyValue("AnalysisExitScript").toString().isEmpty()))
		{
			QString scriptNamePrefix = getScriptNamePrefix();
			ScriptableContainer::runScript(scriptNamePrefix+"Exit");
		}
		break;
	}
}

//Returns true if this object contains a property of the input type, whether or not it is empty.
bool AnalysisScriptContainer::hasScriptPropertyType(ScriptType type)
{
	switch(type)
	{
	case ENTRY:
		return true;
		break;
	case FRAME:
		return propertyContainer_->getProperty("AnalysisFrameScript");
		break;
	case EXIT:
		return propertyContainer_->getProperty("AnalysisExitScript");
		break;
	}
	return false;
}

bool AnalysisScriptContainer::isPartOfSearch(SearchRequest searchRequest)
{
	//If the search is for all analyses
	//OR If the search is for active analyses and this is an active analysis,
	//OR If the search is for inactive analyses and this is an inactive analysis,
	if	(	(searchRequest.group == SearchRequest::ALL_ANALYSES)	
			||	((searchRequest.group & SearchRequest::ACTIVE_ANALYSES) && getDesignConfig()->isAnalysisActive(getAssociateId()))
			||	((searchRequest.group & SearchRequest::INACTIVE_ANALYSES) && !getDesignConfig()->isAnalysisActive(getAssociateId()))
		)
		return true;
	return false;
}

QString AnalysisScriptContainer::getReturnValueError(QString scriptName,const QScriptValue& returnValue)
{
	QString resultError = "";
	if(Property::experimentalValueWasChanged())
	{
		QString parentPath = getParentPath();
		resultError = QString("The %1 script in element: \"%2\" caused the property value: \"%3\" to change.\n"
			"Analysis scripts may only be used to read from, not write to values in experimental elements.  ")
			.arg(scriptName)
			.arg(getParentPath())
			.arg(Property::changedExperimentalValueName());
	}
	if(resultError.isEmpty())
	{
		resultError = ScriptableContainer::getReturnValueError(scriptName,returnValue);
	}
	return resultError;
}

void AnalysisScriptContainer::postDeserialize()
{
	ScriptableContainer::postDeserialize();
}

bool AnalysisScriptContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ScriptableContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

bool AnalysisScriptContainer::hasScripts()
{
	return	!propertyContainer_->getPropertyValue("AnalysisEntryScript").toString().isEmpty() 
			||	(hasScriptPropertyType(FRAME)
				&& !propertyContainer_->getPropertyValue("AnalysisFrameScript").toString().isEmpty())
			|| (hasScriptPropertyType(EXIT) 
				&& !propertyContainer_->getPropertyValue("AnalysisExitScript").toString().isEmpty());
}

QMap<QString,QString> AnalysisScriptContainer::getScripts()
{
	QMap<QString,QString>  scripts = ScriptableContainer::getScripts();
	QString script = propertyContainer_->getPropertyValue("AnalysisEntryScript").toString();
	QString scriptNamePrefix = getScriptNamePrefix();
	if(!script.isEmpty())
	{
		scripts[scriptNamePrefix+"Entry"] = QString("AnalysisEntryScript");
	}
	if(hasScriptPropertyType(FRAME))
	{
		QString script = propertyContainer_->getPropertyValue("AnalysisFrameScript").toString();
		if(!script.isEmpty())
		{
			scripts[scriptNamePrefix+"Frame"] = QString("AnalysisFrameScript");
		}
	}
	if(hasScriptPropertyType(EXIT))
	{
		QString script = propertyContainer_->getPropertyValue("AnalysisExitScript").toString();
		if(!script.isEmpty())
		{
			scripts[scriptNamePrefix+"Exit"] = QString("AnalysisExitScript");
		}
	}
	return scripts;
}

//Since this object gets created as soon as a script is written and sticks around even if that script
//is deleted:
//In the case where its scripts are all empty and a search for its simple existance comes through, 
//it overloads the default behavior and returns false, since it might as well not exist and will
//not be visible in the UI.
bool AnalysisScriptContainer::executeSearchAlgorithm(SearchRequest searchRequest)
{
	if(!hasScripts())
		return false;
	return ScriptableContainer::executeSearchAlgorithm(searchRequest);
}

QString AnalysisScriptContainer::getScriptNamePrefix()
{
	QSharedPointer<Asset> linkedAsset = getLinkedAsset();
	Q_ASSERT(linkedAsset);
	return linkedAsset->getName().simplified().remove(' ')+"Analysis";
}
}; //namespace Picto
