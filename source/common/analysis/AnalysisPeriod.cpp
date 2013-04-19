#include <QScriptValueIterator>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include <QCoreApplication>
#include "AnalysisPeriod.h"

#include "StarterContainer.h"
#include "EnderContainer.h"
#include "ElementTrigger.h"
#include "LFPTrigger.h"
#include "PropertyTrigger.h"
#include "SignalTrigger.h"
#include "SpikeTrigger.h"
#include "TimeTrigger.h"
#include "FrameTrigger.h"
#include "AlignTrigger.h"
#include "TransitionTrigger.h"
#include "AnalysisOutputDep.h"
#include "NumericVariable.h"
#include "FileOutput.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisPeriod::AnalysisPeriod()
{
	AddDefinableProperty(QVariant::Int,"StartBufferMs",0);
	AddDefinableProperty(QVariant::Int,"EndBufferMs",0);
	AddDefinableProperty("Script","");

	QSharedPointer<AssetFactory> triggerFactory(new AssetFactory(0,-1));
	AddDefinableObjectFactory("StarterContainer",QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(StarterContainer::Create))));
	AddDefinableObjectFactory("EnderContainer",QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(EnderContainer::Create))));

	AddDefinableObjectFactory("Trigger",triggerFactory);
	triggerFactory->addAssetType("Element",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ElementTrigger::Create))));	
	triggerFactory->addAssetType("LFP",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LFPTrigger::Create))));	
	triggerFactory->addAssetType("Property",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PropertyTrigger::Create))));
	triggerFactory->addAssetType("Frame",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(FrameTrigger::Create))));
	triggerFactory->addAssetType("Align",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AlignTrigger::Create))));
	triggerFactory->addAssetType("Signal",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SignalTrigger::Create))));	
	triggerFactory->addAssetType("Spike",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SpikeTrigger::Create))));	
	triggerFactory->addAssetType("Time",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TimeTrigger::Create))));	
	triggerFactory->addAssetType("Transition",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TransitionTrigger::Create))));	

	QSharedPointer<AssetFactory> toolFactory(new AssetFactory(0,-1));
	AddDefinableObjectFactory("Tool",toolFactory);
	toolFactory->addAssetType("NumericVariable",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(NumericVariable::Create))));	
	toolFactory->addAssetType("File",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(FileOutput::Create))));	
}

AnalysisPeriod::~AnalysisPeriod()
{
}

QSharedPointer<Asset> AnalysisPeriod::Create()
{
	return QSharedPointer<Asset>(new AnalysisPeriod());
}

void AnalysisPeriod::loadSession(QSqlDatabase session,bool enScriptDebugging)
{
	session_ = session;
	//Setup scripting
	qsEngine_ = QSharedPointer<QScriptEngine>(new QScriptEngine());
	if(enScriptDebugging)
	{
		qsEngineDebugger_ = QSharedPointer<QScriptEngineDebugger>(new QScriptEngineDebugger());
		qsEngineDebugger_->attachTo(qsEngine_.data());
	}


	QList<QSharedPointer<Asset>> triggers = getGeneratedChildren("Trigger");
	QSharedPointer<AnalysisTrigger> trigger;
	foreach(QSharedPointer<Asset> triggerAsset,triggers)
	{
		trigger = triggerAsset.staticCast<AnalysisTrigger>();
		trigger->loadSessionAndScriptTools(session_,qsEngine_);
	}

	QList<QSharedPointer<Asset>> periodDefTriggers;
	periodDefTriggers << getGeneratedChildren("StarterContainer")[0].staticCast<StarterContainer>()->getTriggers();
	periodDefTriggers << getGeneratedChildren("EnderContainer")[0].staticCast<StarterContainer>()->getTriggers();
	foreach(QSharedPointer<Asset> triggerAsset,periodDefTriggers)
	{
		trigger = triggerAsset.staticCast<AnalysisTrigger>();
		trigger->loadSessionAndScriptTools(session_,qsEngine_);
	}

	//Make a Qt Script Function out of the script and its name
	QString function = "function TestScriptName() { " + propertyContainer_->getPropertyValue("Script").toString().toLatin1() + "}";
	//periodScript_ = QScriptProgram(propertyContainer_->getPropertyValue("Script").toString().toLatin1());

	//add the function to the engine by calling evaluate on it
	qsEngine_->evaluate(function);
	//qsEngine_->evaluate(periodScript_);
	//Check for errors
	if(qsEngine_->hasUncaughtException())
	{
		QString errorMsg = "Uncaught exception in " + getName().toLatin1() + " script \n";
		errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
										  .arg(qsEngine_->uncaughtException().toString());
		errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
		QMessageBox box;
		box.setText("Script Error                                      ");
		box.setDetailedText(errorMsg);
		box.setIconPixmap(QPixmap(":/icons/x.png"));
		box.exec();
		return;
	}

	startIndex_ = EventOrderIndex();
	endIndex_ = EventOrderIndex();
	periodNumber_ = 0;
}

void AnalysisPeriod::startNewRun(QString runName,QUuid runUuid)
{
	QList<QSharedPointer<Asset>> analysisTools = getGeneratedChildren("Tool");
	QSharedPointer<AnalysisTool> analysisTool;
	foreach(QSharedPointer<Asset> toolAsset,analysisTools)
	{
		analysisTool = toolAsset.staticCast<AnalysisTool>();
		analysisTool->initialize(runName,runUuid);
		analysisTool->bindToScriptEngine(*qsEngine_);
	}
	periodNumber_ = 0;
}

bool AnalysisPeriod::run(EventOrderIndex fromIndex,EventOrderIndex toIndex)
{
	double scriptTime = 0;
	double otherTime = 0;
	int startBufferMs = propertyContainer_->getPropertyValue("StartBufferMs").toInt();
	int endBufferMs = propertyContainer_->getPropertyValue("EndBufferMs").toInt();
	QTime timer;
	timer.start();
	if(!fromIndex.isValid())	//Invalid toIndex => we don't know when it will end yet.
		return false;

	//Tell triggers that they need to recheck the session
	//database for new data.
	QList<QSharedPointer<Asset>> triggers = getGeneratedChildren("Trigger");
	triggers << getGeneratedChildren("StarterContainer")[0].staticCast<StarterContainer>()->getTriggers();
	triggers << getGeneratedChildren("EnderContainer")[0].staticCast<StarterContainer>()->getTriggers();

	QSharedPointer<AnalysisTrigger> trigger;
	foreach(QSharedPointer<Asset> triggerAsset,triggers)
	{
		trigger = triggerAsset.staticCast<AnalysisTrigger>();
		if(periodNumber_ == 0)
		{
			//If we just started this run, set the data window to all triggers.
			if(toIndex.isValid())
			{
				trigger->setDataWindow(EventOrderIndex(fromIndex.time_-double(startBufferMs*.001)),
										EventOrderIndex(toIndex.time_+double(endBufferMs*.001)));
			}
			else
			{
				trigger->setDataWindow(	startBufferMs?EventOrderIndex(fromIndex.time_-double(startBufferMs*.001)):fromIndex,
									toIndex);

			}

		}
		trigger->sessionDatabaseUpdated();
	}
		
	//The first time that we search for a start index in a run, we look for a start index with a time
	//greater than or equal to the fromIndex rather than checking only the index.  This is because the 
	//input index may be the index from the first frame which would come after the various initial transitions
	//and property changes for that frame.  We don't want to skip those values as possible triggers for this
	//run, so we just look at their time and not their dataid.  After that we are always comparing triggers
	//to previous trigger dataids so we don't look at times any more and do the standard dataid comparison.
	while((startIndex_ < fromIndex) && ((startIndex_.time_ < fromIndex.time_)||(periodNumber_ > 0)))
	{	//Either this is the first run call since startNewRun() or last call finished with
		//no startTrigger.  
		//Try to get a new startIndex_.
		startIndex_ = getGeneratedChildren("StarterContainer")[0].staticCast<StarterContainer>()->getNextTriggerInList(startIndex_);
		if(!startIndex_.isValid())
			return false;
	}
	endIndex_ = getGeneratedChildren("EnderContainer")[0].staticCast<StarterContainer>()->getNextTriggerInList(startIndex_);
	if(!endIndex_.isValid())
		return false;
	
	QTime performTimer;
	performTimer.start();
	//Get data until there are no more triggers or we've passed the input index.
	while( (startIndex_.isValid()) && (endIndex_.isValid() ) 
		&& (!toIndex.isValid() || ((startIndex_ <= toIndex) && (endIndex_ <= toIndex))) )
	{
		//GET DATA AND ADD IT TO SCRIPT ENGINE////////////////////////////////////////
		QList<QSharedPointer<Asset>> triggers = getGeneratedChildren("Trigger");
		QSharedPointer<AnalysisTrigger> trigger;
		foreach(QSharedPointer<Asset> triggerAsset,triggers)
		{
			trigger = triggerAsset.staticCast<AnalysisTrigger>();
			//If there are non-zero buffer values, create new time only indeces, otherwise
			//use the start and end indeces as they are.  This will prevent overlapping
			//of periods for cases where behavioral based triggers are used with no buffers.
			if(startBufferMs)
				trigger->setPeriodStart(EventOrderIndex(startIndex_.time_-double(startBufferMs*.001)));
			else
				trigger->setPeriodStart(startIndex_);
			if(endBufferMs)
				trigger->fillArraysTo(EventOrderIndex(endIndex_.time_+double(endBufferMs*.001)));
			else
				trigger->fillArraysTo(endIndex_);
			QCoreApplication::processEvents();
		}
		//////////////////////////////////////////////////////////////////////////////

		//Add/Update start, end times, and period number as properties to script engine
		qsEngine_->globalObject().setProperty("startTime", startIndex_.time_);
		qsEngine_->globalObject().setProperty("endTime", endIndex_.time_);
		qsEngine_->globalObject().setProperty("periodNumber", periodNumber_);

		otherTime += performTimer.elapsed();
		performTimer.start();

		//RUN SCRIPT//////////////////////////////////////////////////////////////////

		//Run the script
		qsEngine_->globalObject().property("TestScriptName").call().toString();
		//qsEngine_->evaluate(periodScript_);
		if(qsEngine_->hasUncaughtException())
		{
			QString errorMsg = "Uncaught exception in " + getName().toLatin1() + " script \n";
			errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
											  .arg(qsEngine_->uncaughtException().toString());
			errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
			QMessageBox box;
			box.setText("Script Error                                      ");
			box.setDetailedText(errorMsg);
			box.setIconPixmap(QPixmap(":/icons/x.png"));
			box.exec();
			return false;
		}
		scriptTime+=performTimer.elapsed();
		performTimer.start();

		//////////////////////////////////////////////////////////////////////////////

		//Increment period number
		periodNumber_++;
		qDebug(QString("ScriptTime/OtherTime Ratio: %1").arg(scriptTime/otherTime).toLatin1());

		//Get the next period times
		while(startIndex_ < endIndex_)
		{
			startIndex_ = getGeneratedChildren("StarterContainer")[0].staticCast<StarterContainer>()->getNextTriggerInList(startIndex_);
			if(!startIndex_.isValid())
				return true;
		}
		endIndex_ = getGeneratedChildren("EnderContainer")[0].staticCast<StarterContainer>()->getNextTriggerInList(startIndex_);
		if(!endIndex_.isValid())
			return true;
	}
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
		returnVal.append(trigger->scriptInfo()).append("\n");
	}
	return returnVal;
}

void AnalysisPeriod::finishUp()
{
	QList<QSharedPointer<Asset>> analysisTools = getGeneratedChildren("Tool");
	QSharedPointer<AnalysisOutputDep> analysisOutput;
	foreach(QSharedPointer<Asset> toolAsset,analysisTools)
	{
		if(toolAsset->inherits("Picto::AnalysisOutputDep"))
		{
			analysisOutput = toolAsset.staticCast<AnalysisOutputDep>();
			analysisOutput->finishUp();
		}
	}
}

QLinkedList<QPointer<AnalysisOutputWidget>> AnalysisPeriod::getOutputWidgets()
{
	QLinkedList<QPointer<AnalysisOutputWidget>> returnVal;
	QList<QSharedPointer<Asset>> analysisTools = getGeneratedChildren("Tool");
	QSharedPointer<AnalysisOutputDep> outputObj;
	foreach(QSharedPointer<Asset> toolAsset,analysisTools)
	{
		if(toolAsset->inherits("Picto::AnalysisOutputDep"))
		{
			outputObj = toolAsset.staticCast<AnalysisOutputDep>();
			QPointer<AnalysisOutputWidget> outputWidget = outputObj->getOutputWidget();
			if(outputWidget)
				returnVal.append(outputWidget);
		}
	}
	return returnVal;
}

unsigned int AnalysisPeriod::getPercentRemaining()
{
		QList<QSharedPointer<Asset>> triggers = getGeneratedChildren("Trigger");
		int numTriggerObjects = triggers.size();
		QSharedPointer<AnalysisTrigger> trigger;
		float avgFractionRemaining = 0;
		foreach(QSharedPointer<Asset> triggerAsset,triggers)
		{
			trigger = triggerAsset.staticCast<AnalysisTrigger>();
			avgFractionRemaining += trigger->fractionTriggersRemaining()/numTriggerObjects;
		}
		return 100*avgFractionRemaining;
}

void AnalysisPeriod::postDeserialize()
{
	UIEnabled::postDeserialize();
}

bool AnalysisPeriod::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}