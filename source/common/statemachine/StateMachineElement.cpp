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


StateMachineElement::StateMachineElement()
{
	AddDefinableProperty("EntryScript","");
	AddDefinableProperty("ExitScript","");
	AddDefinableObjectFactory("Type",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
}

QString StateMachineElement::slaveRenderFrame(QSharedPointer<Engine::PictoEngine>)
{
	return "";
}

QPoint StateMachineElement::getDisplayLayoutPosition()
{
	return layoutPosition_;
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

void StateMachineElement::runEntryScript()
{
	runAnalysisEntryScripts();
	if(propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty())
		return;
	QString entryScriptName = getName().simplified().remove(' ')+"Entry";
	runScript(entryScriptName);
}

void StateMachineElement::runExitScript()
{
	if(!propertyContainer_->getPropertyValue("ExitScript").toString().isEmpty())
	{
		QString exitScriptName = getName().simplified().remove(' ')+"Exit";
		runScript(exitScriptName);
	}
	runAnalysisExitScripts();
}

void StateMachineElement::runAnalysisEntryScripts()
{
	runAnalysisScripts(StateMachineElement::ENTRY);
}

void StateMachineElement::runAnalysisExitScripts()
{
	runAnalysisScripts(StateMachineElement::EXIT);
}

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

QMap<QString,QString>  StateMachineElement::getScripts()
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

	return scripts;
}

}; //namespace Picto
