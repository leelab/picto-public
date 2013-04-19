#include "Experiment.h"

#include "../engine/PictoEngine.h"
#include "../common/storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

Experiment::Experiment()
:
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

	//Override UIEnabled's Name property.  Experiment objects names must now always be "experiment"
	AddDefinableObjectFactory("Name",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
	//Override UIEnabled for a default value of true
	AddDefinableProperty(QVariant::Bool,"UIEnabled",true);
}

QSharedPointer<Experiment> Experiment::Create()
{
	QSharedPointer<Experiment> newExperiment(new Experiment());
	newExperiment->setSelfPtr(newExperiment);
	newExperiment->setExperimentConfig(QSharedPointer<ExperimentConfig>(new ExperimentConfig()));
	newExperiment->propTable_ = QSharedPointer<PropertyTable>(new PropertyTable(newExperiment->getExperimentConfig()));
	return newExperiment;
}

void Experiment::setEngine(QSharedPointer<Engine::PictoEngine> engine)
{
	engine_ = engine;
	//We call the function below here so that the Gain/Offset values will be
	//set to their initial states (for mouse signal) as early as possible and 
	//values set by the user before running the experiment won't need to be 
	//reset.
	updateSignalCoefficients(NULL,QVariant());
};

QSharedPointer<Engine::PictoEngine> Experiment::getEngine()
{
	return engine_;
}

void Experiment::addTask(QSharedPointer<Task> task)
{
	tasks_.append(task);
	task->setTaskNumber(tasks_.size());
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

	Q_ASSERT(propTable_);
	engine_->setPropertyTable(propTable_);
	engine_->setExperimentConfig(expConfig_);

	//search through tasks_ for a matching task and run it!
	//note that the taskname here may have had all of it's whitespace 
	//removed, so we need to check that possibility
	do
	{
	QSharedPointer<Task> task = getTaskByName(taskName);
	if(!task)
	{
		return false;
	}
	engine_->clearChangedPropertyPackages();
	engine_->play();
	//Start out by sending all starting property values to the server
	engine_->sendAllPropertyValuesToServer();

	engine_->startAllSignalChannels();
	//Initialize signal channel coefficients
	updateSignalCoefficients(NULL,QVariant());
	resetScriptableValues();
	QString result = task->run(engine_);
	engine_->stopAllSignalChannels();

	//If the task changed somehow (ie. We are running as slaves)
	//The prior task would end with a result that is a path 
	//within a different task, if that is the case, re-enter at that task.
	QStringList pathElems = result.split("::",QString::SkipEmptyParts);
	if(pathElems.size() <= 1)
		taskName.clear();
	else
		taskName = pathElems[0];

	} while(!taskName.isEmpty());

	engine_->setPropertyTable(QSharedPointer<PropertyTable>());
	engine_->setExperimentConfig(QSharedPointer<ExperimentConfig>());

	return true;
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

void Experiment::postDeserialize()
{
	signalCoeffInitialized_ = false;
	expConfig_->disallowIdDuplication();
	//Usually an object's parent adds itself to the objects scriptables list.
	//Since an experiment is at the top level, we must do it manually.
	addScriptable(selfPtr().staticCast<Scriptable>());
	ScriptableContainer::postDeserialize();

	QString experimentSyntaxVer = propertyContainer_->getPropertyValue("SyntaxVersion").toString();
	if(experimentSyntaxVer != DESIGNSYNTAXVERSION)
	{
		expConfig_->setDeserializedVersion(experimentSyntaxVer);
		upgradeVersion(experimentSyntaxVer);
		propertyContainer_->setPropertyValue("SyntaxVersion",DESIGNSYNTAXVERSION);
	}
	propertyContainer_->getProperty("SyntaxVersion")->setVisible(false);


	//Set the signal properties runtime editable
	//We use the DataStore version of this function so that the actual properties,
	//not the init properties, are the ones affected by changing the properties
	//from the workstation.  If we didn't do this, the init properties would change
	//which would have no effect the experiment is not part of the state machine's
	//run system and the values are never copied.
	DataStore::setPropertyRuntimeEditable("XOffset");
	DataStore::setPropertyRuntimeEditable("XGain");
	DataStore::setPropertyRuntimeEditable("YOffset");
	DataStore::setPropertyRuntimeEditable("YGain");
	DataStore::setPropertyRuntimeEditable("XYSignalShear");
	
	//clear out the existing experiment
	tasks_.clear();
	QList<QSharedPointer<Asset>> newTasks = getGeneratedChildren("Task");
	foreach(QSharedPointer<Asset> newTask,newTasks)
	{
		sortTasksIntoList(newTask);
	}
	//In case tasks are added after deserialization (ie. in the state machine editor).  Attach the childAdded
	//signal to the sortTasksIntoList slot.
	connect(this,SIGNAL(childAddedAfterDeserialize(QSharedPointer<Asset>)),this,SLOT(sortTasksIntoList(QSharedPointer<Asset>)));

	//Add all descendant properties to the property table for reporting of property changes to server.
	//engine->setLastTimePropertiesRequested(0);	//If this is slave mode, this will assure that
	//											//we get all properties that have been changed
	//											//since director started.
	propTable_->clear();//Empties property table so director/viewer props will match up
	QList<QSharedPointer<Property>> descendantProps = getDescendantsProperties();
	foreach(QSharedPointer<Property> prop,descendantProps)
	{
		propTable_->addProperty(prop);	// This adds the property to the property table and gives it an index for use in transmission
	}

	//Experiment objects are not part of the state machine. Init values should be bypassed and set immediately as run values in all
	//properties
	QHash<QString, QVector<QSharedPointer<Property>>> propMap = propertyContainer_->getProperties();
	foreach(QVector<QSharedPointer<Property>> propVec, propMap)
	{
		foreach(QSharedPointer<Property> prop, propVec)
		{
			prop->enableInitRunValueSync(true);
		}
	}
}

bool Experiment::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ScriptableContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

void Experiment::updateSignalCoefficients(Property*,QVariant)
{
	if(engine_.isNull())
		return;
	QSharedPointer<SignalChannel> posChannel = engine_->getSignalChannel("Position");
	QRect windowDims = engine_->getRenderingTargets().first()->getVisualTarget()->getDimensions();
	int displayWidth = 800;
	int displayHeight = 600;

	if(!signalCoeffInitialized_)
	{
		signalCoeffInitialized_ = true;
		//If we're using a mouse channel, ignore the saved signal parameters since we know how to set them up automatically.
		//We will still allow these values to be adjusted by the user later though for testing purposes.
		bool isMouseChannel = posChannel->getPortType() == "mouse";
		if(isMouseChannel)
		{	QRect windowDims = engine_->getRenderingTargets().first()->getVisualTarget()->getDimensions();
			propertyContainer_->setPropertyValue("XOffset",-(displayWidth/2));
			propertyContainer_->setPropertyValue("XGain",1.0);
			propertyContainer_->setPropertyValue("YOffset",-(displayHeight/2));
			propertyContainer_->setPropertyValue("YGain",1.0);
			propertyContainer_->setPropertyValue("XYSignalShear",0.0);
		}
		//connect the signal properties to the updateSignalCoefficients function
		connect(propertyContainer_->getProperty("XOffset").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(updateSignalCoefficients(Property*,QVariant)));
		connect(propertyContainer_->getProperty("XGain").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(updateSignalCoefficients(Property*,QVariant)));
		connect(propertyContainer_->getProperty("YOffset").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(updateSignalCoefficients(Property*,QVariant)));
		connect(propertyContainer_->getProperty("YGain").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(updateSignalCoefficients(Property*,QVariant)));
		connect(propertyContainer_->getProperty("XYSignalShear").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(updateSignalCoefficients(Property*,QVariant)));
	}
	double xZoom = propertyContainer_->getPropertyValue("XGain").toDouble();
	double yZoom = propertyContainer_->getPropertyValue("YGain").toDouble();
	double xGain = xZoom;
	double yGain = yZoom;
	int xOffset = displayWidth/2 + propertyContainer_->getPropertyValue("XOffset").toDouble()*xZoom;
	int yOffset = displayHeight/2 + propertyContainer_->getPropertyValue("YOffset").toDouble()*yZoom;

	posChannel->setCalibrationCoefficients("x",xGain,xOffset,displayWidth/2);
	posChannel->setCalibrationCoefficients("y",yGain,yOffset,displayHeight/2);
	posChannel->setShear("x","y",propertyContainer_->getPropertyValue("XYSignalShear").toDouble());

}

void Experiment::sortTasksIntoList(QSharedPointer<Asset> newChild)
{
	if(newChild->inherits("Picto::Task"))
		addTask(newChild.staticCast<Task>());
}

}; //namespace Picto
