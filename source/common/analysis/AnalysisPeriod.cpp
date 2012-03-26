#include <QScriptValueIterator>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include <QCoreApplication>
#include "AnalysisPeriod.h"

#include "ElementTrigger.h"
#include "LFPTrigger.h"
#include "PropertyTrigger.h"
#include "SignalTrigger.h"
#include "SpikeTrigger.h"
#include "TimeTrigger.h"
#include "TransitionTrigger.h"
#include "AnalysisTool.h"
#include "FileOutput.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisPeriod::AnalysisPeriod()
{
	AddDefinableProperty(QVariant::Int,"StartBufferMs",0);
	AddDefinableProperty("StartTrigger","");
	AddDefinableProperty("EndTrigger","");
	AddDefinableProperty(QVariant::Int,"EndBufferMs",0);
	AddDefinableProperty("Script","");

	QSharedPointer<AssetFactory> triggerFactory(new AssetFactory(0,-1));
	AddDefinableObjectFactory("Trigger",triggerFactory);
	triggerFactory->addAssetType("Element",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ElementTrigger::Create))));	
	triggerFactory->addAssetType("LFP",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LFPTrigger::Create))));	
	triggerFactory->addAssetType("Property",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PropertyTrigger::Create))));	
	triggerFactory->addAssetType("Signal",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SignalTrigger::Create))));	
	triggerFactory->addAssetType("Spike",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SpikeTrigger::Create))));	
	triggerFactory->addAssetType("Time",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TimeTrigger::Create))));	
	triggerFactory->addAssetType("Transition",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TransitionTrigger::Create))));	
	
	QSharedPointer<AssetFactory> outputFactory(new AssetFactory(0,-1));
	AddDefinableObjectFactory("Output",outputFactory);
	outputFactory->addAssetType("File",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(FileOutput::Create))));	
	reset();
}

AnalysisPeriod::~AnalysisPeriod()
{
}

QSharedPointer<Asset> AnalysisPeriod::Create()
{
	return QSharedPointer<Asset>(new AnalysisPeriod());
}

void AnalysisPeriod::loadSession(QSqlDatabase session)
{
	session_ = session;
	QList<QSharedPointer<Asset>> triggers = getGeneratedChildren("Trigger");
	QSharedPointer<AnalysisTrigger> trigger;
	foreach(QSharedPointer<Asset> triggerAsset,triggers)
	{
		trigger = triggerAsset.staticCast<AnalysisTrigger>();
		trigger->loadSession(session_);
	}
}

void AnalysisPeriod::reset()
{
	QList<QSharedPointer<Asset>> triggers = getGeneratedChildren("Trigger");
	QSharedPointer<AnalysisTrigger> trigger;
	foreach(QSharedPointer<Asset> triggerAsset,triggers)
	{
		trigger = triggerAsset.staticCast<AnalysisTrigger>();
		trigger->reset();
	}
	QList<QSharedPointer<Asset>> analysisTools = getGeneratedChildren("Output");
	QSharedPointer<AnalysisTool> analysisTool;
	foreach(QSharedPointer<Asset> toolAsset,analysisTools)
	{
		analysisTool = toolAsset.staticCast<AnalysisTool>();
		analysisTool->reset();
	}
	startIndex_ = EventOrderIndex();
	endIndex_ = EventOrderIndex();
	periodNumber_ = 0;
}

bool AnalysisPeriod::runTo(double time)
{
	//Tell triggers that they need to recheck the session
	//database for new data.
	QList<QSharedPointer<Asset>> triggers = getGeneratedChildren("Trigger");
	QSharedPointer<AnalysisTrigger> trigger;
	foreach(QSharedPointer<Asset> triggerAsset,triggers)
	{
		trigger = triggerAsset.staticCast<AnalysisTrigger>();
		trigger->sessionDatabaseUpdated();
	}
		

	QString returnVal;
	if(!startIndex_.isValid())
	{	//Either this is the first runTo() call since reset() or last call finished with
		//no startTrigger.  
		//Try to get a new startIndex_.
		startIndex_ = startTrigger_->getNextTriggerTime();
		if(!startIndex_.isValid())
			return false;
	}
	while(endIndex_ <= startIndex_)
	{	//Either this is the first runTo() call since reset() or last call finished with
		//no endTrigger.
		//Try to get a new endIndex_.
		if(endTrigger_)
			endIndex_ = endTrigger_->getNextTriggerTime();
		else
			endIndex_ = startTrigger_->getNextTriggerTime();
		if(!endIndex_.isValid())
			return false;
	}
	
	QTime timer;
	timer.start();
	//Get data until there are no more triggers or we've passed the input time.
	while( (startIndex_.isValid()) && (endIndex_.isValid() ) 
		&& ((time < 0)||((startIndex_.time_ <= time) && (endIndex_.time_ <= time))) )
	{
		//Currently we build a new script engine for every period.  This is not the most
		//efficient way to do it, but I want to get this all working before I figure out how
		//to clear out old script objects from the script engine.
		QSharedPointer<QScriptEngine> qsEngine = QSharedPointer<QScriptEngine>(new QScriptEngine());

		//GET DATA AND ADD IT TO SCRIPT ENGINE////////////////////////////////////////
		QList<QSharedPointer<Asset>> triggers = getGeneratedChildren("Trigger");
		QSharedPointer<AnalysisTrigger> trigger;
		int totalTriggers = 0;
		int totalRemaining = 0;
		foreach(QSharedPointer<Asset> triggerAsset,triggers)
		{
			trigger = triggerAsset.staticCast<AnalysisTrigger>();
			if((trigger == startTrigger_) || (trigger == endTrigger_))
				continue;
			//If there are non-zero buffer values, create new time only indeces, otherwise
			//use the start and end indeces as they are.  This will prevent overlapping
			//of periods for cases where behavioral based triggers are used with no buffers.
			int startBufferMs = propertyContainer_->getPropertyValue("StartBufferMs").toInt();
			int endBufferMs = propertyContainer_->getPropertyValue("EndBufferMs").toInt();
			if(startBufferMs)
				trigger->setPeriodStart(EventOrderIndex(startIndex_.time_-double(startBufferMs*.001)));
			else
				trigger->setPeriodStart(startIndex_);
			if(endBufferMs)
				trigger->fillArraysTo(EventOrderIndex(endIndex_.time_+double(endBufferMs*.001)));
			else
				trigger->fillArraysTo(endIndex_);
			trigger->addDataSourcesToScriptEngine(qsEngine);
			
			//Update total known/remaining triggers and emit progress
			totalTriggers += trigger->totalKnownTriggers();
			totalRemaining += trigger->remainingKnownTriggers();

			if(timer.elapsed() > 100)
			{	//If things take a long time, process events periodically.
				QCoreApplication::processEvents();
				timer.start();
			}
		}
		if(totalTriggers > 0)
			emit percentRemaining(100*totalRemaining/totalTriggers);
		//////////////////////////////////////////////////////////////////////////////

		//Add start, end times, and period number as properties to script engine
		qsEngine->globalObject().setProperty("startTime", startIndex_.time_);
		qsEngine->globalObject().setProperty("endTime", endIndex_.time_);
		qsEngine->globalObject().setProperty("periodNumber", periodNumber_);

		//Add AnalysisTool objects to script engine
		QList<QSharedPointer<Asset>> analysisTools = getGeneratedChildren("Output");
		QSharedPointer<AnalysisTool> analysisTool;
		foreach(QSharedPointer<Asset> toolAsset,analysisTools)
		{
			analysisTool = toolAsset.staticCast<AnalysisTool>();
			analysisTool->bindToScriptEngine(*qsEngine);
		}

		//RUN SCRIPT//////////////////////////////////////////////////////////////////
		//Make a Qt Script Function out of the script and its name
		QString function = "function TestScriptName() { " + propertyContainer_->getPropertyValue("Script").toString().toAscii() + "}";

		//add the function to the engine by calling evaluate on it
		qsEngine->evaluate(function);
		//Check for errors
		if(qsEngine->hasUncaughtException())
		{
			QString errorMsg = "Uncaught exception in " + getName().toAscii() + " script \n";
			errorMsg += QString("Line %1: %2\n").arg(qsEngine->uncaughtExceptionLineNumber())
											  .arg(qsEngine->uncaughtException().toString());
			errorMsg += QString("Backtrace: %1\n").arg(qsEngine->uncaughtExceptionBacktrace().join(", "));
						QMessageBox box;
			box.setText("Script Error                                      ");
			box.setDetailedText(errorMsg);
			box.setIconPixmap(QPixmap(":/icons/x.png"));
			box.exec();
			return false;
		}

		//Run the script
		qsEngine->globalObject().property("TestScriptName").call().toString();
		if(qsEngine->hasUncaughtException())
		{
			QString errorMsg = "Uncaught exception in " + getName().toAscii() + " script \n";
			errorMsg += QString("Line %1: %2\n").arg(qsEngine->uncaughtExceptionLineNumber())
											  .arg(qsEngine->uncaughtException().toString());
			errorMsg += QString("Backtrace: %1\n").arg(qsEngine->uncaughtExceptionBacktrace().join(", "));
			QMessageBox box;
			box.setText("Script Error                                      ");
			box.setDetailedText(errorMsg);
			box.setIconPixmap(QPixmap(":/icons/x.png"));
			box.exec();
			return false;
		}
		

		//////////////////////////////////////////////////////////////////////////////

		//Increment period number
		periodNumber_++;

		//Get the next period times
		if((startTrigger_ == endTrigger_) || !endTrigger_)
		{
			startIndex_ = endIndex_;
			endIndex_ = startTrigger_->getNextTriggerTime();
		}
		else
		{
			startIndex_ = startTrigger_->getNextTriggerTime();
			do
			{
				endIndex_ = endTrigger_->getNextTriggerTime();
				if(!endIndex_.isValid())
					return true;
			}while(endIndex_ <= startIndex_);
		}
	}
	return true;
}

void AnalysisPeriod::finishUp()
{
	QList<QSharedPointer<Asset>> analysisTools = getGeneratedChildren("Output");
	QSharedPointer<AnalysisTool> analysisTool;
	foreach(QSharedPointer<Asset> toolAsset,analysisTools)
	{
		analysisTool = toolAsset.staticCast<AnalysisTool>();
		analysisTool->finishUp();
	}
}

QLinkedList<QPointer<QWidget>> AnalysisPeriod::getOutputWidgets()
{
	QLinkedList<QPointer<QWidget>> returnVal;
	QList<QSharedPointer<Asset>> analysisTools = getGeneratedChildren("Output");
	QSharedPointer<AnalysisTool> outputObj;
	foreach(QSharedPointer<Asset> outputAsset,analysisTools)
	{
		outputObj = outputAsset.staticCast<AnalysisTool>();
		returnVal.append(outputObj->getOutputWidget());
	}
	return returnVal;
}

void AnalysisPeriod::postDeserialize()
{
	UIEnabled::postDeserialize();
	reset();
	QList<QSharedPointer<Asset>> triggers = getGeneratedChildren("Trigger");
	QString startTriggerName = propertyContainer_->getPropertyValue("StartTrigger").toString();
	QString endTriggerName = propertyContainer_->getPropertyValue("EndTrigger").toString();
	QSharedPointer<AnalysisTrigger> trigger;
	foreach(QSharedPointer<Asset> triggerAsset,triggers)
	{
		trigger = triggerAsset.staticCast<AnalysisTrigger>();
		if(trigger->getName() == startTriggerName)
		{
			startTrigger_ = trigger;
		}
		if(trigger->getName() == endTriggerName)
		{
			endTrigger_ = trigger;
		}
	}
}

bool AnalysisPeriod::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}

QString AnalysisPeriod::scriptInfo()
{
	QString returnVal;
	QList<QSharedPointer<Asset>> triggers = getGeneratedChildren("Trigger");
	QSharedPointer<AnalysisTrigger> trigger;
	foreach(QSharedPointer<Asset> triggerAsset,triggers)
	{
		trigger = triggerAsset.staticCast<AnalysisTrigger>();
		if((trigger == startTrigger_) || (trigger == endTrigger_))
			continue;
		returnVal.append(trigger->scriptInfo()).append("\n");
	}
	return returnVal;
}