#include <QList>
#include "AnalysisTrigger.h"
#include "AnalysisDataSource.h"

#include "LFPDataSource.h"
#include "PropertyDataSource.h"
#include "SignalDataSource.h"
#include "SpikeDataSource.h"
#include "TimeDataSource.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisTrigger::AnalysisTrigger()
{
	QSharedPointer<AssetFactory> dataSourceFactory(new AssetFactory(0,-1));
	AddDefinableObjectFactory("DataSource",dataSourceFactory);
	dataSourceFactory->addAssetType("LFP",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LFPDataSource::Create))));	
	dataSourceFactory->addAssetType("Property",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PropertyDataSource::Create))));	
	dataSourceFactory->addAssetType("Signal",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SignalDataSource::Create))));	
	dataSourceFactory->addAssetType("Spike",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SpikeDataSource::Create))));	
	dataSourceFactory->addAssetType("Time",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TimeDataSource::Create))));	
	reset();
}

AnalysisTrigger::~AnalysisTrigger()
{}

void AnalysisTrigger::loadSession(QSqlDatabase session)
{
	session_ = session;
	QList<QSharedPointer<Asset>> dataSources = getGeneratedChildren("DataSource");
	QSharedPointer<AnalysisDataSource> dataSource;
	foreach(QSharedPointer<Asset> dataSourceAsset,dataSources)
	{
		dataSource = dataSourceAsset.staticCast<AnalysisDataSource>();
		dataSource->loadSession(session_);
	}
}

void AnalysisTrigger::reset()
{
	periodData_.clear();
	nextPeriodsDataLoc_ = periodData_.end();
	QList<QSharedPointer<Asset>> dataSources = getGeneratedChildren("DataSource");
	QSharedPointer<AnalysisDataSource> dataSource;
	foreach(QSharedPointer<Asset> dataSourceAsset,dataSources)
	{
		dataSource = dataSourceAsset.staticCast<AnalysisDataSource>();
		dataSource->restart();
	}
}

void AnalysisTrigger::setPeriodStart(EventOrderIndex startIndex)
{
	Q_ASSERT_X(periodStart_ <= startIndex,"AnalysisTrigger::setPeriodStart",
		"AnalysisTrigger start indeces must be greater than the start indeces of their predecessors.");
	periodStart_ = startIndex;
}

void AnalysisTrigger::fillArraysTo(EventOrderIndex beforeIndex)
{
	Q_ASSERT_X(beforeIndex >= periodStart_,"AnalysisTrigger::fillArraysTo","period window is undefined.");
	if(!periodData_.size() || (periodData_.last().index < beforeIndex))
	{
		//We don't yet have a trigger event that is up to beforeIndex.
		//Keep grabbing trigger times and data from the data sources
		//at those times until their are no more trigger times left
		//or the trigger index is equal to the beforeIndex input.
		EventOrderIndex triggerIndex;
		while(true)
		{
			triggerIndex = getNextTriggerTime();
			if(triggerIndex.time_ < 0)	
				break;	//Looks like there are no more triggers

			//We got a new trigger time.
			//Get values from all the DataSources.  Put them in a dataBlock
			//and append it with the trigger time to periodData_.
			TriggerData newData;
			QList<QSharedPointer<Asset>> dataSources = getGeneratedChildren("DataSource");
			newData.dataBlock.resize(dataSources.size());
			newData.index = triggerIndex;
			QSharedPointer<AnalysisDataSource> dataSource;
			int i=0;
			foreach(QSharedPointer<Asset> dataSourceAsset,dataSources)
			{
				dataSource = dataSourceAsset.staticCast<AnalysisDataSource>();
				newData.dataBlock[i].name = dataSource->getName();
				newData.dataBlock[i++].value = dataSource->getValue(triggerIndex);
			}
			periodData_.append(newData);
			//If the periodData_ list was empty, initialize the nextPeriodsDataLoc_
			//to the first entry in the list.
			if(periodData_.size() == 1)
				nextPeriodsDataLoc_ = periodData_.begin();
				
		}
	}
	//Set nextPeriodsDataLoc_ to the first entry with index equal to or greater than beforeIndex
	while(nextPeriodsDataLoc_ != periodData_.end() 
		&& (nextPeriodsDataLoc_->index < beforeIndex))
	{	
		nextPeriodsDataLoc_++;
	}
	//Erase any data that's before period start
	//Remove everything before startIndex.
	QLinkedList<TriggerData>::iterator it;
	for(it = periodData_.begin();it != periodData_.end();it++)
	{
		if(it->index >=periodStart_)
		{
			periodData_.erase(periodData_.begin(),it);
			break;
		}
	}
}

void AnalysisTrigger::addScriptableArrayToEngine(QSharedPointer<QScriptEngine> qsEngine)
{
	QLinkedList<QScriptValue> scriptRowList;
	QLinkedList<TriggerData>::iterator it;
	for(it = periodData_.begin();
		(it != nextPeriodsDataLoc_) && (it != periodData_.end());
		it++)
	{
		QScriptValue scriptArrayRow = qsEngine->newObject();
		foreach(SourceDataUnit dataUnit,it->dataBlock)
		{
			scriptArrayRow.setProperty(dataUnit.name,dataUnit.value);
		}
		scriptRowList.push_back(scriptArrayRow);
	}
	QScriptValue dataArray = qsEngine->newArray(scriptRowList.size());
	int i=0;
	foreach(QScriptValue scriptArrayRow,scriptRowList)
	{
		dataArray.setProperty(QString::number(i++),scriptArrayRow);
	}
	qsEngine->globalObject().setProperty(getName(),dataArray);
}

QString AnalysisTrigger::scriptInfo()
{
	QString returnVal = QString("\t%1 - Values:").arg(getName());
	QList<QSharedPointer<Asset>> dataSources = getGeneratedChildren("DataSource");
	QSharedPointer<AnalysisDataSource> dataSource;
	foreach(QSharedPointer<Asset> dataSourceAsset,dataSources)
	{
		dataSource = dataSourceAsset.staticCast<AnalysisDataSource>();
		returnVal.append(QString("\n\t\t%1").arg(dataSource->getName()));
	}
	return returnVal;
}

void AnalysisTrigger::postDeserialize()
{
	UIEnabled::postDeserialize();
}

bool AnalysisTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}