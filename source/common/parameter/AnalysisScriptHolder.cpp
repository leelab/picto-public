#include "AnalysisScriptHolder.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new AnalysisScriptHolder.
 *	\details Adds three Properties, AnalysisEntryScript, AnalysisFrameScript, AnalysisExitScript
 */
AnalysisScriptHolder::AnalysisScriptHolder()
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

/*! brief Creates a new AnalysisScriptHolder and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> AnalysisScriptHolder::Create()
{
	return QSharedPointer<Asset>(new AnalysisScriptHolder());
}
/*! \brief Runs the input script type if it exists.*/
void AnalysisScriptHolder::runScript(ScriptType type)
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
		if(propertyContainer_->getProperty("AnalysisFrameScript") 
			&& !propertyContainer_->getProperty("AnalysisFrameScript")->isDeleted()
			&& (!propertyContainer_->getPropertyValue("AnalysisFrameScript").toString().isEmpty()))
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

/*! \brief Returns true if this object contains a Property of the input type, whether or not it is empty.*/
bool AnalysisScriptHolder::hasScriptPropertyType(ScriptType type)
{
	switch(type)
	{
	case ENTRY:
		return true;
		break;
	case FRAME:
		return propertyContainer_->getProperty("AnalysisFrameScript") 
			&& !propertyContainer_->getProperty("AnalysisFrameScript")->isDeleted();
		break;
	case EXIT:
		return propertyContainer_->getProperty("AnalysisExitScript");
		break;
	}
	return false;
}

bool AnalysisScriptHolder::isPartOfSearch(SearchRequest searchRequest)
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

/*! \brief Emits a runtime error if an Analysis Script causes an Experimental Property value to change.
 *	\details Analyses should not change Experimental Property values since it is analyzing, not affecting
 *	the run.  If it does so then some kind of design error occured and the integrity of the Analysis output
 *	may be compromised, so we emit a runtime error.
*/
QString AnalysisScriptHolder::getReturnValueError(QString scriptName,const QScriptValue& returnValue)
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

void AnalysisScriptHolder::postDeserialize()
{
	ScriptableContainer::postDeserialize();
	QSharedPointer<AssociateHostLink> lnk = getGeneratedChildren("HostLink").first().staticCast<AssociateHostLink>();
	connect(lnk.data(),SIGNAL(linkedToAsset(QSharedPointer<Asset>)),this,SLOT(linkedToAsset(QSharedPointer<Asset>)));
}

bool AnalysisScriptHolder::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ScriptableContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

bool AnalysisScriptHolder::hasScripts()
{
	return	!propertyContainer_->getPropertyValue("AnalysisEntryScript").toString().isEmpty() 
			||	(hasScriptPropertyType(FRAME)
				&& !propertyContainer_->getPropertyValue("AnalysisFrameScript").toString().isEmpty()
				&& !propertyContainer_->getProperty("AnalysisFrameScript")->isDeleted())
			|| (hasScriptPropertyType(EXIT) 
				&& !propertyContainer_->getPropertyValue("AnalysisExitScript").toString().isEmpty());
}

QMap<QString,QString> AnalysisScriptHolder::getScripts()
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

/*!	\brief Extends the search algorithm so that we don't have false positives in certain types of searches.
 *	\details Since this object gets created as soon as a script is written and sticks around even if that script
 * is deleted, in cases where its scripts are all empty and a search for its simple existance comes through, 
 * we need to overload the default behavior and return false. If all scripts are empty, this element might as well 
 * not exist and should not be considered a relavent AnalysisElement in the UI.
 */
bool AnalysisScriptHolder::executeSearchAlgorithm(SearchRequest searchRequest)
{
	if(!hasScripts())
		return false;
	return ScriptableContainer::executeSearchAlgorithm(searchRequest);
}

/*! \brief Creates an appropriate prefix for names of this elements scripts.
 *	\details The prefix is something like "LinkedAssetNameAnalysis" meaning that an Exit script would have
 *	the name "LinkedAssetNameAnalysisExit".
 */
QString AnalysisScriptHolder::getScriptNamePrefix()
{
	QSharedPointer<Asset> linkedAsset = getLinkedAsset();
	Q_ASSERT(linkedAsset);
	return linkedAsset->getName().simplified().remove(' ')+"Analysis";
}

/*! \brief Called when this AssociateElement links to a design element.  Deletes the AnalysisFrameScript if this element
 *	didn't attach to a State.
 */
void AnalysisScriptHolder::linkedToAsset(QSharedPointer<Asset> asset)
{
	//If this script container contains a frame script and is not linked to a state, erase
	//the frame script
	if(asset->inherits("Picto::State"))
		return;
	if(asset->inherits("Picto::StateMachineElement") && hasScriptPropertyType(FRAME))
	{
		propertyContainer_->getProperty("AnalysisFrameScript")->setVisible(false);
		propertyContainer_->getProperty("AnalysisFrameScript")->setDeleted();
	}

}
}; //namespace Picto
