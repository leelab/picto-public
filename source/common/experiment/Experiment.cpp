#include "Experiment.h"

#include "../engine/PictoEngine.h"

namespace Picto {

Experiment::Experiment()
:
latestSyntaxVersion_("0.0.1"),
engine_(NULL)
{
	signalCoeffInitialized_ = false;
	AddDefinableProperty("SyntaxVersion","");
	AddDefinableObjectFactory("Task",QSharedPointer<AssetFactory>(new AssetFactory(1,-1,AssetFactory::NewAssetFnPtr(Picto::Task::Create))));
	AddDefinableProperty(QVariant::Double,"XGain",1.0);
	AddDefinableProperty(QVariant::Double,"YGain",1.0);
	AddDefinableProperty(QVariant::Int,"XOffset",0);
	AddDefinableProperty(QVariant::Int,"YOffset",0);
	AddDefinableProperty(QVariant::Double,"XYSignalShear",0);
}

QSharedPointer<Experiment> Experiment::Create()
{
	QSharedPointer<Experiment> newExperiment(new Experiment());
	newExperiment->setSelfPtr(newExperiment);
	return newExperiment;
}

void Experiment::setEngine(QSharedPointer<Engine::PictoEngine> engine)
{
	Q_ASSERT(propTable_);
	engine_ = engine;
	engine_->setPropertyTable(propTable_);
};

void Experiment::addTask(QSharedPointer<Task> task)
{
	tasks_.append(task);
}

QStringList Experiment::getTaskNames()
{
	QStringList taskList;
	foreach(QSharedPointer<Task> task, tasks_)
	{
		taskList.append(task->getName());
	}
	return taskList;
}

QSharedPointer<Task> Experiment::getTaskByName(QString taskName)
{
	QSharedPointer<Task> returnVal;
	if(tasks_.isEmpty())
		return returnVal;
	//search through tasks_ for a matching task!
	//note that the taskname here may have had all of it's whitespace 
	//removed, so we need to check that possibility
	foreach(QSharedPointer<Task> task, tasks_)
	{
		if(task->getName() == taskName ||
		   task->getName().simplified().remove(' ') == taskName)
		{	
			returnVal = task;
			break;
		}
	}
	return returnVal;
}

bool Experiment::runTask(QString taskName)
{
	if(engine_.isNull())
		return false;
	if(tasks_.isEmpty())
		return false;

	//search through tasks_ for a matching task and run it!
	//note that the taskname here may have had all of it's whitespace 
	//removed, so we need to check that possibility
	QSharedPointer<Task> task = getTaskByName(taskName);
	if(!task)
		return false;
	engine_->clearChangedPropertyPackage();
	engine_->clearEngineCommand();
	//Start out by sending all starting property values to the server
	engine_->sendAllPropertyValuesToServer();

	engine_->startAllSignalChannels();
	//Initialize signal channel coefficients
	updateSignalCoefficients(QSharedPointer<Property>());
	bool success = task->run(engine_);
	engine_->stopAllSignalChannels();
	return success;
	//foreach(QSharedPointer<Task> task, tasks_)
	//{
	//	if(task->getName() == taskName ||
	//	   task->getName().simplified().remove(' ') == taskName)
	//	{	
	//		engine->clearEngineCommand();
	//		engine->startAllSignalChannels();
	//		bool success = task->run(engine);
	//		engine->stopAllSignalChannels();

	//		return success;
	//	}
	//}
	//return false;
}


//! Jumps to the specified task and state
bool Experiment::jumpToState(QStringList path, QString state)
{
	QString taskName = path.takeFirst();
	foreach(QSharedPointer<Task> task, tasks_)
	{
		if(task->getName() == taskName ||
		   task->getName().simplified().remove(' ') == taskName)
		{	
			task->jumpToState(path,state);
			return true;
		}
	}
	return false;
}

void Experiment::postSerialize()
{
	UIEnabled::postSerialize();
	QString experimentSyntaxVer = propertyContainer_->getPropertyValue("SyntaxVersion").toString();
	if(experimentSyntaxVer == "")
		propertyContainer_->setPropertyValue("SyntaxVersion",latestSyntaxVersion_);

	//connect the signal properties to the updateSignalCoefficients function
	connect(propertyContainer_->getProperty("XOffset").data(),SIGNAL(valueChanged(QSharedPointer<Property>)),this,SLOT(updateSignalCoefficients(QSharedPointer<Property>)));
	connect(propertyContainer_->getProperty("XGain").data(),SIGNAL(valueChanged(QSharedPointer<Property>)),this,SLOT(updateSignalCoefficients(QSharedPointer<Property>)));
	connect(propertyContainer_->getProperty("YOffset").data(),SIGNAL(valueChanged(QSharedPointer<Property>)),this,SLOT(updateSignalCoefficients(QSharedPointer<Property>)));
	connect(propertyContainer_->getProperty("YGain").data(),SIGNAL(valueChanged(QSharedPointer<Property>)),this,SLOT(updateSignalCoefficients(QSharedPointer<Property>)));
	connect(propertyContainer_->getProperty("XYSignalShear").data(),SIGNAL(valueChanged(QSharedPointer<Property>)),this,SLOT(updateSignalCoefficients(QSharedPointer<Property>)));
	//Set the signal properties runtime editable
	setPropertyRuntimeEditable("XOffset");
	setPropertyRuntimeEditable("XGain");
	setPropertyRuntimeEditable("YOffset");
	setPropertyRuntimeEditable("YGain");
	setPropertyRuntimeEditable("XYSignalShear");
	
	//clear out the existing experiment
	tasks_.clear();
	QList<QSharedPointer<Asset>> newTasks = getGeneratedChildren("Task");
	foreach(QSharedPointer<Asset> newTask,newTasks)
	{
		addTask(newTask.staticCast<Task>());
	}

	//Add all descendant properties to the property table for reporting of property changes to server.
	//engine->setLastTimePropertiesRequested(0);	//If this is slave mode, this will assure that
	//											//we get all properties that have been changed
	//											//since director started.
	propTable_ = QSharedPointer<PropertyTable>(new PropertyTable());
	propTable_->clear();//Empties property table so director/viewer props will match up
	QList<QSharedPointer<Property>> descendantProps = getDescendantsProperties();
	foreach(QSharedPointer<Property> prop,descendantProps)
	{
		propTable_->addProperty(prop);	// This adds the property to the property table and gives it an index for use in transmission
	}
}

bool Experiment::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}

void Experiment::updateSignalCoefficients(QSharedPointer<Property>)
{
	if(engine_.isNull())
		return;
	QSharedPointer<SignalChannel> posChannel = engine_->getSignalChannel("PositionChannel");
	QRect windowDims = engine_->getRenderingTargets().first()->getVisualTarget()->getDimensions();

	if(!signalCoeffInitialized_)
	{
		signalCoeffInitialized_ = true;
		//If we're using a mouse channel, ignore the saved signal parameters since we know how to set them up automatically.
		//We will still allow these values to be adjusted by the user later though for testing purposes.
		bool isMouseChannel = posChannel->inherits("Picto::MouseSignalChannel");
		if(isMouseChannel)
		{	QRect windowDims = engine_->getRenderingTargets().first()->getVisualTarget()->getDimensions();
			//propertyContainer_->setPropertyValue("XSignalLeftEdge",0);
			//propertyContainer_->setPropertyValue("XSignalRightEdge",windowDims.width());
			//propertyContainer_->setPropertyValue("YSignalTopEdge",0);
			//propertyContainer_->setPropertyValue("YSignalBottomEdge",windowDims.height());
			//propertyContainer_->setPropertyValue("XYSignalShear",0);
			propertyContainer_->setPropertyValue("XOffset",0);
			propertyContainer_->setPropertyValue("XGain",1.0);
			propertyContainer_->setPropertyValue("YOffset",0);
			propertyContainer_->setPropertyValue("YGain",1.0);
			propertyContainer_->setPropertyValue("XYSignalShear",0.0);
		}
	}
	posChannel->setCalibrationCoefficients("xpos",
											propertyContainer_->getPropertyValue("XGain").toDouble(),
													propertyContainer_->getPropertyValue("XOffset").toInt(),
													400/*double(windowDims.width())/2.0*/);//The value here before is more correct, but currently everything assumes 800x600 so we'll do that here too.
	posChannel->setCalibrationCoefficients("ypos",
													propertyContainer_->getPropertyValue("YGain").toDouble(),
													propertyContainer_->getPropertyValue("YOffset").toInt(),
													300/*double(windowDims.height())/2.0*/);//The value here before is more correct, but currently everything assumes 800x600 so we'll do that here too.
	posChannel->setShear("xpos","ypos",propertyContainer_->getPropertyValue("XYSignalShear").toDouble());

}

}; //namespace Picto
