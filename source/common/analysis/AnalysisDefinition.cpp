#include <QMessageBox>
#include <QSqlQuery>
#include "AnalysisDefinition.h"
#include "AnalysisOutput.h"
#include "NumericVariable.h"
#include "FileOutput.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisDefinition::AnalysisDefinition()
{
	QSharedPointer<AssetFactory> periodFactory(new AssetFactory(0,-1));
	AddDefinableProperty("Script","");
	AddDefinableObjectFactory("Period",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisPeriod::Create))));	

	QSharedPointer<AssetFactory> toolFactory(new AssetFactory(0,-1));
	AddDefinableObjectFactory("Tool",toolFactory);
	toolFactory->addAssetType("NumericVariable",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(NumericVariable::Create))));	
	toolFactory->addAssetType("File",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(FileOutput::Create))));	

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
	qsEngine_ = QSharedPointer<QScriptEngine>(new QScriptEngine());

	QList<QSharedPointer<Asset>> analysisTools = getGeneratedChildren("Tool");
	QSharedPointer<AnalysisTool> analysisTool;
	foreach(QSharedPointer<Asset> toolAsset,analysisTools)
	{
		analysisTool = toolAsset.staticCast<AnalysisTool>();
		analysisTool->reset();
		analysisTool->bindToScriptEngine(*qsEngine_);
	}

	QList<QSharedPointer<Asset>> periods = getGeneratedChildren("Period");
	QSharedPointer<AnalysisPeriod> period;
	foreach(QSharedPointer<Asset> periodAsset,periods)
	{
		period = periodAsset.staticCast<AnalysisPeriod>();
		period->reset();
	}

	//Make a Qt Script Function out of the script and its name
	QString function = "function TestScriptName() { " + propertyContainer_->getPropertyValue("Script").toString().toAscii() + "}";

	//add the function to the engine by calling evaluate on it
	qsEngine_->evaluate(function);
	//Check for errors
	if(qsEngine_->hasUncaughtException())
	{
		QString errorMsg = "Uncaught exception in " + getName().toAscii() + " script \n";
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

	currRunNum_ = 0;
	currPeriodNum_ = 0;
}

void AnalysisDefinition::startNewRun(QSharedPointer<TaskRunDataUnit> runInfo)
{
	Q_ASSERT(runInfo);
	currRun_ = runInfo;
	currRunNum_++;
	QList<QSharedPointer<Asset>> periods = getGeneratedChildren("Period");
	QSharedPointer<AnalysisPeriod> period;
	foreach(QSharedPointer<Asset> periodAsset,periods)
	{
		period = periodAsset.staticCast<AnalysisPeriod>();
		period->startNewRun(currRun_->name_);
	}

	QList<QSharedPointer<Asset>> analysisTools = getGeneratedChildren("Tool");
	QSharedPointer<AnalysisOutput> analysisOutput;
	QSharedPointer<AnalysisTool> analysisTool;
	foreach(QSharedPointer<Asset> toolAsset,analysisTools)
	{
		if(toolAsset->inherits("Picto::AnalysisOutput"))
		{
			analysisOutput = toolAsset.staticCast<AnalysisOutput>();
			analysisOutput->setOutputNamePrefix(currRun_->name_);
		}
		if(toolAsset->inherits("Picto::AnalysisTool"))
		{
			analysisTool = toolAsset.staticCast<AnalysisTool>();
			analysisTool->reset();
		}

	}
}

bool AnalysisDefinition::run()
{
	double startTime = getFrameTime(currRun_->startFrame_);
	double stopTime = getFrameTime(currRun_->endFrame_);
	EventOrderIndex fromIndex(
		startTime,
		currRun_->startFrame_,
		Picto::EventOrderIndex::BEHAVIORAL
	);
	EventOrderIndex toIndex = (stopTime >= 0)?EventOrderIndex(
		stopTime,
		currRun_->endFrame_,
		Picto::EventOrderIndex::BEHAVIORAL
		):EventOrderIndex();

	QList<QSharedPointer<Asset>> periods = getGeneratedChildren("Period");
	currPeriodNum_ = 0;
	foreach(QSharedPointer<Asset> periodAsset,periods)
	{
		currPeriod_ = periodAsset.staticCast<AnalysisPeriod>();
		currPeriod_->run(fromIndex,toIndex);
		currPeriodNum_++;
	}

	//Add/Update start, end times, and period number as properties to script engine
	qsEngine_->globalObject().setProperty("name", currRun_->name_);
	qsEngine_->globalObject().setProperty("notes", currRun_->notes_);
	qsEngine_->globalObject().setProperty("startTime", startTime);
	qsEngine_->globalObject().setProperty("endTime", stopTime);
	qsEngine_->globalObject().setProperty("runNumber", currRunNum_);

	//RUN SCRIPT//////////////////////////////////////////////////////////////////

	//Run the script
	qsEngine_->globalObject().property("TestScriptName").call().toString();
	if(qsEngine_->hasUncaughtException())
	{
		QString errorMsg = "Uncaught exception in " + getName().toAscii() + " script \n";
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

	QList<QSharedPointer<Asset>> analysisTools = getGeneratedChildren("Tool");
	QSharedPointer<AnalysisOutput> analysisOutput;
	foreach(QSharedPointer<Asset> toolAsset,analysisTools)
	{
		if(toolAsset->inherits("Picto::AnalysisOutput"))
		{
			analysisOutput = toolAsset.staticCast<AnalysisOutput>();
			analysisOutput->finishUp();
		}
	}
}

//bool AnalysisDefinition::saveOutputToDirectory(QString directory, QString filename)
//{
//	bool returnVal = true;
//	QList<QSharedPointer<Asset>> periods = getGeneratedChildren("Period");
//	QSharedPointer<AnalysisPeriod> period;
//	foreach(QSharedPointer<Asset> periodAsset,periods)
//	{
//		period = periodAsset.staticCast<AnalysisPeriod>();
//		returnVal  = returnVal & period->saveOutputToDirectory(directory,filename);
//	}
//	if(periods.size() == 0)
//		returnVal = false;
//	return returnVal;
//}

QLinkedList<QPointer<AnalysisOutputWidget>> AnalysisDefinition::getOutputWidgets()
{
	QLinkedList<QPointer<AnalysisOutputWidget>> returnVal;

	QList<QSharedPointer<Asset>> analysisTools = getGeneratedChildren("Tool");
	QSharedPointer<AnalysisOutput> outputObj;
	foreach(QSharedPointer<Asset> toolAsset,analysisTools)
	{
		if(toolAsset->inherits("Picto::AnalysisOutput"))
		{
			outputObj = toolAsset.staticCast<AnalysisOutput>();
			returnVal.append(outputObj->getOutputWidget());
		}
	}

	QList<QSharedPointer<Asset>> periods = getGeneratedChildren("Period");
	QSharedPointer<AnalysisPeriod> period;
	foreach(QSharedPointer<Asset> periodAsset,periods)
	{
		period = periodAsset.staticCast<AnalysisPeriod>();
		returnVal << period->getOutputWidgets();
	}

	return returnVal;
}

unsigned int AnalysisDefinition::getPercentRemaining()
{
	if(!currPeriod_)
		return 100;
	unsigned int periodPercentRemaining = currPeriod_->getPercentRemaining();
	int totalPercentRemaining = 0;
	if(numPeriods_ > 0)
	{
		totalPercentRemaining = ( (numPeriods_-currPeriodNum_ - 1)*100 
							+ periodPercentRemaining)/numPeriods_;
	}
	return totalPercentRemaining;
}

void AnalysisDefinition::postDeserialize()
{
	UIEnabled::postDeserialize();
	QList<QSharedPointer<Asset>> periods = getGeneratedChildren("Period");
	numPeriods_ = periods.size();
	reset();
}

bool AnalysisDefinition::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}

double AnalysisDefinition::getFrameTime(qulonglong frameId)
{
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	//Get property value list.
	query.prepare("SELECT time FROM frames WHERE dataid=:frameid");
	query.bindValue(":frameid",frameId);
	bool success = query.exec();
	if(!success || !query.next())
	{
		return -1;
	}
	return query.value(0).toDouble();
}