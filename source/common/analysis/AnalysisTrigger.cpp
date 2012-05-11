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
}

AnalysisTrigger::~AnalysisTrigger()
{}

void AnalysisTrigger::loadSessionAndScriptTools(QSqlDatabase session,QSharedPointer<QScriptEngine> qsEngine)
{
	session_ = session;
	qsEngine_ = qsEngine;
	if(qsEngine)
		triggerArray_ = qsEngine->newArray();
	else
		triggerArray_ = QScriptValue();
	periodData_.clear();
	periodStart_ = EventOrderIndex();
	nextPeriodsDataLoc_ = periodData_.end();
	currentTriggerIndex_ = EventOrderIndex();
	latestValue_.clear();
	dataIterator_.clear();
	dataIterator_ = createDataIterator();
	QList<QSharedPointer<Asset>> dataSources = getGeneratedChildren("DataSource");
	QSharedPointer<AnalysisDataSource> dataSource;
	foreach(QSharedPointer<Asset> dataSourceAsset,dataSources)
	{
		dataSource = dataSourceAsset.staticCast<AnalysisDataSource>();
		dataSource->loadSessionAndScriptTools(session_,qsEngine,triggerArray_);
	}

	if(qsEngine)
		qsEngine->globalObject().setProperty(getName(),triggerArray_);
}

void AnalysisTrigger::setDataWindow(EventOrderIndex startFrom,EventOrderIndex endBefore)
{
	dataIterator_->setDataWindow(startFrom,endBefore);
	QList<QSharedPointer<Asset>> dataSources = getGeneratedChildren("DataSource");
	QSharedPointer<AnalysisDataSource> dataSource;
	foreach(QSharedPointer<Asset> dataSourceAsset,dataSources)
	{
		dataSource = dataSourceAsset.staticCast<AnalysisDataSource>();
		dataSource->setDataWindow(startFrom,endBefore);
	}
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
	latestValue_ = dataIterator_->getNextValue();
	currentTriggerIndex_ = latestValue_->index;
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
	if(!dataIterator_)
		return;
	dataIterator_->sessionDatabaseUpdated();
}

float AnalysisTrigger::fractionTriggersRemaining()
{
	if(!dataIterator_) return 1.0;
	return dataIterator_->fractionRemaining();
}

QSharedPointer<AnalysisValue> AnalysisTrigger::getLatestValue()
{
	return latestValue_;
}

QString AnalysisTrigger::getIteratorDescriptor()
{
	if(!dataIterator_)
		return "";
	return QString(dataIterator_->metaObject()->className())+"-Desc-"+dataIterator_->propertyDescriptor();
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