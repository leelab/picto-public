#include <QCoreApplication>
#include "StateMachineElement.h"

#include "../property/Property.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"
#include "../storage/StateDataUnit.h"
#include "../storage/AssetFactory.h"
#include "../parameter/AnalysisScriptHolder.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {


/*! \brief Constructs a StateMachineElement object.
 *	\details Adds EntryScript and ExitScript Properties that define the scripts that are called when control flow enters and exits this
 *	StateMachineElement.
 */
StateMachineElement::StateMachineElement()
{
	AddDefinableProperty("EntryScript","");
	AddDefinableProperty("ExitScript","");
	AddDefinableObjectFactory("Type",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));

	setForAlignment_ = false;
	setForSelEvent_ = false;
	setForMarker_ = false;
}

/*! \brief In any StateMachineElement where experimental time passes (currently State and PausePoint) this function causes the latest frame
 *	to be rendered.  In other StateMachineElements it doesn't do anything.
 *	\details This is used by the SlaveExperimentDriver to render frames whenever it sees that control flow remains in a particular state
 *	during the course of a frame presentation.
 */
QString StateMachineElement::slaveRenderFrame(QSharedPointer<Engine::PictoEngine>)
{
	return "";
}

void StateMachineElement::postDeserialize()
{
	ResultContainer::postDeserialize();
}

bool StateMachineElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ResultContainer::validateObject(xmlStreamReader))
		return false;
	QString resultName;
	QStringList results = getResultList();
	foreach(QString result,results)
	{
		if(result == "EngineAbort")
		{
			addError("EngineAbort is a resticted keyword, and may not be used as the name of a result");
			return false;
		}
	}
	return true;
}

bool StateMachineElement::hasScripts()
{
	if(ResultContainer::hasScripts())
		return true;
	return (!propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty()
		|| !propertyContainer_->getPropertyValue("ExitScript").toString().isEmpty());
}

/*! \brief Runs this StateMachineElement's EntryScript.
 *	\details If there is an attached AnalysisEntryScript (in the case of the TestViewer), it is run before the regular EntryScript.
*/
void StateMachineElement::runEntryScript()
{
	runAnalysisEntryScripts();
	if(propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty())
		return;
	QString entryScriptName = getName().simplified().remove(' ')+"Entry";
	runScript(entryScriptName);
}

/*! \brief Runs this StateMachineElement's ExitScript.
 *	\details If there is an attached AnalysisExitScript (in the case of the TestViewer), it is run after the regular ExitScript.
*/
void StateMachineElement::runExitScript()
{
	if(!propertyContainer_->getPropertyValue("ExitScript").toString().isEmpty())
	{
		QString exitScriptName = getName().simplified().remove(' ')+"Exit";
		runScript(exitScriptName);
	}
	runAnalysisExitScripts();
}

/*! \brief Runs this any active AnalysisEntryScripts attached to this StateMachineElement.
*/
void StateMachineElement::runAnalysisEntryScripts()
{
	runAnalysisScripts(StateMachineElement::ENTRY);
}

/*! \brief Runs this any active AnalysisExitScripts attached to this StateMachineElement.
*/
void StateMachineElement::runAnalysisExitScripts()
{
	runAnalysisScripts(StateMachineElement::EXIT);
}

/*! \brief Runs any active AnalysisScripts that are attached to this StateMachineElement and have the input ScriptType.
*/
void StateMachineElement::runAnalysisScripts(ScriptType type)
{
	QList<QUuid> activeAnalysisIds = getDesignConfig()->getActiveAnalysisIds();
	if(activeAnalysisIds.isEmpty())
		return;
	QList<QSharedPointer<Asset>> analysisScriptChildren;
	QSharedPointer<AnalysisScriptHolder> scriptContainer;
	foreach(QUuid analysisId,activeAnalysisIds)
	{
		analysisScriptChildren = getAssociateChildren(analysisId,"AnalysisScriptHolder");
		if(analysisScriptChildren.isEmpty())
			continue;
		scriptContainer = analysisScriptChildren.first().staticCast<AnalysisScriptHolder>();
		scriptContainer->runScript(AnalysisScriptHolder::ScriptType(type));
	}
}

QMap<QString,QString> StateMachineElement::getScripts()
{
	QMap<QString,QString> scripts = ResultContainer::getScripts();

	if(!propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"Entry";
		scripts[scriptName] = QString("EntryScript");
	}

	if(!propertyContainer_->getPropertyValue("ExitScript").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"Exit";
		scripts[scriptName] = QString("ExitScript");
	}

	return scripts;
}
void StateMachineElement::setAlignParam(bool alignParam)
{
	if (alignParam == true)
		setForAlignment_ = true;
	else
		setForAlignment_ = false;
}
bool StateMachineElement::getAlignParam()
{
	return setForAlignment_;
}
void StateMachineElement::setSelEventParam(bool setSelEvt)
{
	if (setSelEvt == true)
		setForSelEvent_ = true;
	else
		setForSelEvent_ = false;
}
bool StateMachineElement::getSelEventParam()
{
	return setForSelEvent_;
}
void StateMachineElement::setMarkerParam(bool markerParam)
{
	if (markerParam == true)
		setForMarker_ = true;
	else
		setForMarker_ = false;
}
bool StateMachineElement::getMarkerParam()
{
	return setForMarker_;
}
}; //namespace Picto
