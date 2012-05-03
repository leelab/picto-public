#include <QList>
#include <QTime>
#include <QCoreApplication>
#include "AnalysisTrigger.h"
#include "AnalysisDataSource.h"

#include "LFPDataSource.h"
#include "PropertyDataSource.h"
#include "SignalDataSource.h"
#include "SpikeDataSource.h"
#include "TimeDataSource.h"
#include "FrameDataSource.h"
#include "ElementDataSource.h"
#include "DefaultDataSource.h"

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
	dataSourceFactory->addAssetType("Frame",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(FrameDataSource::Create))));	
	dataSourceFactory->addAssetType("Signal",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SignalDataSource::Create))));	
	dataSourceFactory->addAssetType("Spike",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SpikeDataSource::Create))));	
	dataSourceFactory->addAssetType("Element",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ElementDataSource::Create))));
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
	currentTriggerIndex_ = EventOrderIndex();
}

void AnalysisTrigger::setPeriodStart(EventOrderIndex startIndex)
{
	Q_ASSERT_X(periodStart_ <= startIndex,"AnalysisTrigger::setPeriodStart",
		"AnalysisTrigger start indeces must be greater than the start indeces of their predecessors.");
	periodStart_ = startIndex;

	QLinkedList<TriggerData>::iterator iter = periodData_.begin();
	while(iter != periodData_.end() && (*iter).index < startIndex)
		iter++;
	periodData_.erase(periodData_.begin(),iter);
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
		QTime timer;
		timer.start();
		while(triggerIndex < beforeIndex)
		{
			triggerIndex = getNextTrigger();
			if(!triggerIndex.isValid())	
				break;	//Looks like there are no more triggers


			//We got a new trigger time.
			//If the new trigger time is before periodStart_, continue.
			if(triggerIndex < periodStart_)
				continue;
			
			QScriptValue currScript = qsEngine_->newObject();
			//This trigger falls beyond periodStart_.  Tell the data sources to store it.  
			QList<QSharedPointer<Asset>> dataSources = getGeneratedChildren("DataSource");
			QSharedPointer<AnalysisDataSource> dataSource;
			foreach(QSharedPointer<Asset> dataSourceAsset,dataSources)
			{
				dataSource = dataSourceAsset.staticCast<AnalysisDataSource>();
				dataSource->storeValue(currScript,triggerIndex);
			}
			
			periodData_.append(TriggerData(triggerIndex,currScript));

			if(timer.elapsed() > 100)
			{	//Reading in data can take a long time.  If it does, process events periodically.
				QCoreApplication::processEvents();
				timer.start();
			}
		}
	}


	//Update triggerArray with latest data.
	int i=0;
	foreach(TriggerData value,periodData_)
	{
		if(value.index >= beforeIndex)
			break;
		triggerArray_.setProperty(i++,value.rowScript);
	}
	triggerArray_.setProperty("length",i);
}

void AnalysisTrigger::setScriptEngine(QSharedPointer<QScriptEngine> qsEngine)
{
	qsEngine_ = qsEngine;
	triggerArray_ = qsEngine->newArray();
	QList<QSharedPointer<Asset>> dataSources = getGeneratedChildren("DataSource");
	QSharedPointer<AnalysisDataSource> dataSource;
	foreach(QSharedPointer<Asset> dataSourceAsset,dataSources)
	{
		dataSource = dataSourceAsset.staticCast<AnalysisDataSource>();
		dataSource->setScriptObjects(qsEngine,triggerArray_);
	}
	qsEngine->globalObject().setProperty(getName(),triggerArray_);


	//QScriptValue triggerObject = qsEngine->newQObject(this,QScriptEngine::QtOwnership);
	//QList<QSharedPointer<Asset>> dataSources = getGeneratedChildren("DataSource");
	//QSharedPointer<AnalysisDataSource> dataSource;
	//foreach(QSharedPointer<Asset> dataSourceAsset,dataSources)
	//{
	//	dataSource = dataSourceAsset.staticCast<AnalysisDataSource>();
	//	QScriptValue dataSourceObject = qsEngine->newQObject(dataSource.data(),QScriptEngine::QtOwnership);
	//	triggerObject.setProperty(dataSource->getName(),dataSourceObject);
	//}
	//qsEngine->globalObject().setProperty(getName(),triggerObject);

	//QLinkedList<QScriptValue> scriptRowList;
	//QLinkedList<TriggerData>::iterator it;
	//for(it = periodData_.begin();
	//	(it != nextPeriodsDataLoc_) && (it != periodData_.end());
	//	it++)
	//{
	//	QScriptValue scriptArrayRow = qsEngine->newObject();
	//	foreach(SourceDataUnit dataUnit,it->dataBlock)
	//	{
	//		scriptArrayRow.setProperty(dataUnit.name,dataUnit.value);
	//	}
	//	scriptRowList.push_back(scriptArrayRow);
	//}
	//QScriptValue dataArray = qsEngine->newArray(scriptRowList.size());
	//int i=0;
	//foreach(QScriptValue scriptArrayRow,scriptRowList)
	//{
	//	dataArray.setProperty(QString::number(i++),scriptArrayRow);
	//}
	//qsEngine->globalObject().setProperty(getName(),dataArray);
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

EventOrderIndex AnalysisTrigger::getNextTrigger()
{
	currentTriggerIndex_ = getNextTriggerTime();
	return currentTriggerIndex_;
}

EventOrderIndex AnalysisTrigger::getCurrentTrigger()
{
	return currentTriggerIndex_;
}

void AnalysisTrigger::sessionDatabaseUpdated()
{
	QList<QSharedPointer<Asset>> dataSources = getGeneratedChildren("DataSource");
	QSharedPointer<AnalysisDataSource> dataSource;
	foreach(QSharedPointer<Asset> dataSourceAsset,dataSources)
	{
		dataSource = dataSourceAsset.staticCast<AnalysisDataSource>();
		dataSource->sessionDatabaseUpdated();
	}
	recheckSessionData();
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