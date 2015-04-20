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
	AddDefinableProperty("OperatorEntryScript", "");
	AddDefinableProperty("OperatorExitScript", "");
	AddDefinableObjectFactory("Type",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
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
		|| !propertyContainer_->getPropertyValue("ExitScript").toString().isEmpty()
		|| !propertyContainer_->getPropertyValue("OperatorEntryScript").toString().isEmpty()
		|| !!propertyContainer_->getPropertyValue("OperatorExitScript").toString().isEmpty());
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

/*! \brief Runs this StateMachineElement's OperatorEntryScript.
 *	\details These scripts are run on SlaveExperimentDrivers to update the operator's view.
 */
void StateMachineElement::runOperatorEntryScript()
{
	if(propertyContainer_->getPropertyValue("OperatorEntryScript").toString().isEmpty())
		return;
	QString operatorEntryScriptName = getName().simplified().remove(' ')+"OperatorEntry";
	runScript(operatorEntryScriptName);
}

/*! \brief Runs this StateMachineElement's OperatorExitScript.
 *	\details These scripts are run on SlaveExperimentDrivers to update the operator's view.
 */
void StateMachineElement::runOperatorExitScript()
{
	if(!propertyContainer_->getPropertyValue("OperatorExitScript").toString().isEmpty())
	{
		QString operatorExitScriptName = getName().simplified().remove(' ')+"OperatorExit";
		runScript(operatorExitScriptName);
	}
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
	QMap<QString,QString>  scripts = ResultContainer::getScripts();

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

	if(!propertyContainer_->getPropertyValue("OperatorEntryScript").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"OperatorEntry";
		scripts[scriptName] = QString("OperatorEntryScript");
	}

	if(!propertyContainer_->getPropertyValue("OperatorExitScript").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"OperatorExit";
		scripts[scriptName] = QString("OperatorExitScript");
	}

	return scripts;
}

}; //namespace Picto
