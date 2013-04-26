#include <QMessageBox>
#include <QSqlQuery>
#include "AnalysisContainer.h"
#include "AnalysisDefinition.h"
#include "../storage/ObsoleteAsset.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisContainer::AnalysisContainer()
{
	AddDefinableObjectFactory("AnalysisDefinition",QSharedPointer<AssetFactory>(new AssetFactory(1,-1,AssetFactory::NewAssetFnPtr(AnalysisDefinition::Create))));	

	requireUniqueName(false);
	ASSOCIATE_ROOT_HOST_INITIALIZATION
}

AnalysisContainer::~AnalysisContainer()
{
}

QSharedPointer<Asset> AnalysisContainer::Create()
{
	return QSharedPointer<Asset>(new AnalysisContainer());
}

void AnalysisContainer::loadSession(QSqlDatabase session,bool enScriptDebugging)
{
	session_ = session;

	if(!session_.isValid() || !session_.isOpen())
	{
		QMessageBox box;
		box.setText("Session Error                                      ");
		box.setDetailedText("This input session was invalid.");
		box.setIconPixmap(QPixmap(":/icons/x.png"));
		box.exec();
		return;
	}

	QList<QSharedPointer<Asset>> analyses = getGeneratedChildren("AnalysisDefinition");
	foreach(QSharedPointer<Asset> analysis,analyses)
	{
		analysis.staticCast<AnalysisDefinition>()->loadSession(session_,enScriptDebugging);
	}
}

int AnalysisContainer::numAnalysisDefs()
{
	return getGeneratedChildren("AnalysisDefinition").size();
}

QString AnalysisContainer::analysisDefName(int index)
{
	QList<QSharedPointer<Asset>> analyses  = getGeneratedChildren("AnalysisDefinition");
	if(analyses.size() > index)
		return analyses[index]->getName();
	return "";
}

void AnalysisContainer::activateAnalysis(int index)
{
	if(numAnalysisDefs() <= index)
		return;
	while(index >= activatedAnalyses_.size())
		activatedAnalyses_.push_back(false);
	activatedAnalyses_[index] = true;
}

void AnalysisContainer::deactivateAllAnalyses()
{
	activatedAnalyses_.clear();
}

void AnalysisContainer::startNewRun(QSharedPointer<TaskRunDataUnit> runInfo)
{
	Q_ASSERT(runInfo);
	QList<QSharedPointer<Asset>> analyses = getGeneratedChildren("AnalysisDefinition");
	foreach(QSharedPointer<Asset> analysis,analyses)
	{
		analysis.staticCast<AnalysisDefinition>()->startNewRun(runInfo);
	}
}

bool AnalysisContainer::run()
{
	if(!session_.isValid() || !session_.isOpen())
		return false;

	QList<QSharedPointer<Asset>> analyses = getGeneratedChildren("AnalysisDefinition");
	foreach(QSharedPointer<Asset> analysis,analyses)
	{
		if(!analysis.staticCast<AnalysisDefinition>()->run())
			return false;
	}
	return true;
}

void AnalysisContainer::finish()
{
	QList<QSharedPointer<Asset>> analyses = getGeneratedChildren("AnalysisDefinition");
	foreach(QSharedPointer<Asset> analysis,analyses)
	{
		analysis.staticCast<AnalysisDefinition>()->finish();
	}
}

QLinkedList<QPointer<AnalysisOutputWidget>> AnalysisContainer::getOutputWidgets()
{
	QLinkedList<QPointer<AnalysisOutputWidget>> returnVal;

	QList<QSharedPointer<Asset>> analyses = getGeneratedChildren("AnalysisDefinition");
	foreach(QSharedPointer<Asset> analysis,analyses)
	{
		returnVal << analysis.staticCast<AnalysisDefinition>()->getOutputWidgets();
	}

	return returnVal;
}

unsigned int AnalysisContainer::getPercentRemaining()
{
	float totPercent = numAnalysisDefs()*100;
	float currPercent = 0;
	QList<QSharedPointer<Asset>> analyses = getGeneratedChildren("AnalysisDefinition");
	foreach(QSharedPointer<Asset> analysis,analyses)
	{
		currPercent += analysis.staticCast<AnalysisDefinition>()->getPercentRemaining();
	}
	return int(100.0*currPercent/totPercent);
}

void AnalysisContainer::postDeserialize()
{
	UIEnabled::postDeserialize();
	ASSOCIATE_ROOT_HOST_POST_DESERIALIZE
}

bool AnalysisContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}