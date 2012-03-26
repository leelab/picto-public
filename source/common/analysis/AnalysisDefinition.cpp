#include "AnalysisDefinition.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisDefinition::AnalysisDefinition()
{
	QSharedPointer<AssetFactory> periodFactory(new AssetFactory(0,-1));
	AddDefinableObjectFactory("Period",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisPeriod::Create))));	
	reset();
}

AnalysisDefinition::~AnalysisDefinition()
{
}

void AnalysisDefinition::loadSession(QSqlDatabase session)
{
	session_ = session;
	QList<QSharedPointer<Asset>> periods = getGeneratedChildren("Period");
	QSharedPointer<AnalysisPeriod> period;
	foreach(QSharedPointer<Asset> periodAsset,periods)
	{
		period = periodAsset.staticCast<AnalysisPeriod>();
		period->loadSession(session_);
	}
}

void AnalysisDefinition::reset()
{
	QList<QSharedPointer<Asset>> periods = getGeneratedChildren("Period");
	QSharedPointer<AnalysisPeriod> period;
	foreach(QSharedPointer<Asset> periodAsset,periods)
	{
		period = periodAsset.staticCast<AnalysisPeriod>();
		period->reset();
	}
	currPeriod_ = 0;
}

bool AnalysisDefinition::runTo(double time)
{
	QList<QSharedPointer<Asset>> periods = getGeneratedChildren("Period");
	QSharedPointer<AnalysisPeriod> period;
	currPeriod_ = 0;
	foreach(QSharedPointer<Asset> periodAsset,periods)
	{
		period = periodAsset.staticCast<AnalysisPeriod>();
		period->runTo(time);
		currPeriod_++;
	}
	return true;
}

void AnalysisDefinition::finish()
{
		QList<QSharedPointer<Asset>> periods = getGeneratedChildren("Period");
	QSharedPointer<AnalysisPeriod> period;
	foreach(QSharedPointer<Asset> periodAsset,periods)
	{
		period = periodAsset.staticCast<AnalysisPeriod>();
		period->finishUp();
	}
}

QLinkedList<QPointer<QWidget>> AnalysisDefinition::getOutputWidgets()
{
	QLinkedList<QPointer<QWidget>> returnVal;
	QList<QSharedPointer<Asset>> periods = getGeneratedChildren("Period");
	QSharedPointer<AnalysisPeriod> period;
	foreach(QSharedPointer<Asset> periodAsset,periods)
	{
		period = periodAsset.staticCast<AnalysisPeriod>();
		returnVal << period->getOutputWidgets();
	}
	return returnVal;
}

void AnalysisDefinition::postDeserialize()
{
	UIEnabled::postDeserialize();
	reset();
	QList<QSharedPointer<Asset>> periods = getGeneratedChildren("Period");
	QSharedPointer<AnalysisPeriod> period;
	foreach(QSharedPointer<Asset> periodAsset,periods)
	{
		period = periodAsset.staticCast<AnalysisPeriod>();
		connect(period.data(),SIGNAL(percentRemaining(int)),this,SLOT(updateProgressBar(int)));
	}
	numPeriods_ = periods.size();
}

bool AnalysisDefinition::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}

void AnalysisDefinition::updateProgressBar(int periodPercentRemaining)
{
	int totalPercentRemaining = 0;
	if(numPeriods_ > 0)
	{
		totalPercentRemaining = ( (numPeriods_-currPeriod_ - 1)*100 
							+ periodPercentRemaining)/numPeriods_;
	}
	emit percentRemaining(totalPercentRemaining);
}