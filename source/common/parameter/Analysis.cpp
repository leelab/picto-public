#include "Analysis.h"
#include "AnalysisDataSource.h"
#include "AnalysisFunction.h"
#include "AnalysisScriptContainer.h"
#include "AnalysisVar.h"
#include "AnalysisVarHash.h"
#include "AnalysisVarList.h"
#include "AnalysisOutput.h"
#include "../memleakdetect.h"

namespace Picto {

Analysis::Analysis()
:	ScriptableContainer(),
	variableFactory_(new AssetFactory(0,-1)),
	outputFactory_(new AssetFactory(0,-1)),
	dataSourceFactory_(new AssetFactory(0,-1)),
	functionFactory_(new AssetFactory(0,-1)),
	scriptFactory_(new AssetFactory(0,-1))
{
	AddDefinableProperty("SyntaxVersion","");
	AddDefinableProperty(QVariant::Uuid,"AnalysisId",QVariant());
	AddDefinableProperty(QVariant::Uuid,"LinkedTaskId",QVariant());
	AddDefinableProperty(QVariant::Uuid,"LinkedTaskName",QVariant());

	AddDefinableObjectFactory("AnalysisVariable",variableFactory_);
	variableFactory_->addAssetType("Var",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisVar::Create))));
	variableFactory_->addAssetType("VarList",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisVarList::Create))));
	variableFactory_->addAssetType("VarHash",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisVarHash::Create))));
	
	AddDefinableObjectFactory("AnalysisDataSource",outputFactory_);
	//dataSourceFactory_->addAssetType("Spike",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisSpikeSource::Create))));

	AddDefinableObjectFactory("AnalysisOutput",dataSourceFactory_);
	//outputFactory_->addAssetType("File",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisFileOutput::Create))));

	AddDefinableObjectFactory("AnalysisFunction",functionFactory_);
	functionFactory_->addAssetType("",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisFunction::Create))));

	AddDefinableObjectFactory("AnalysisScriptContainer",scriptFactory_);
	scriptFactory_->addAssetType("",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AnalysisScriptContainer::Create))));
}

QSharedPointer<Asset> Analysis::Create()
{
	QSharedPointer<Analysis> newAnalysis(new Analysis());
	newAnalysis->setSelfPtr(newAnalysis);
	return newAnalysis;
}

//If the experiment contains the task that this analysis is supposed to link to it returns it,
//otherwise it returns an empty shared pointer.
QSharedPointer<Task> Analysis::getLinkableTask(QSharedPointer<Experiment> experiment)
{
	foreach(QString taskName,experiment->getTaskNames())
	{
		if(experiment->getTaskByName(taskName)->getTaskId() == getLinkedTaskId())
			return experiment->getTaskByName(taskName);
	}
	return QSharedPointer<Task>();
}

//Links this analysis to a Task.
//Returns true on success, false if the linkage wasn't perfect.  On false, the feedback
//message has more information.
//THIS IS GOING TO NEED SOME DEBUGGING.  MAKE SURE, FOR EXAMPLE, THAT ADDING AN ANALYSISSCRIPTCONTAINER TO A
//NUMERIC PARAMETER WON'T BREAK ANYTHING!!!
bool Analysis::LinkToTask(QSharedPointer<Task> task, QString& feedback)
{
	feedback = "";
	AnalysisElement* analysisElement;
	bool linkedTaskMatches = task->getTaskId() == getLinkedTaskId();
	QStringList pathMatchedAnalysisAssets;
	QStringList unmatchedAnalysisAssets;
	//Set this task as our linked task
	setLinkedTask(task);
	foreach(QString childTag,getValidChildTags())
	{
		foreach(QSharedPointer<Asset> child,getGeneratedChildren(childTag))
		{
			analysisElement = dynamic_cast<AnalysisElement*>(child.data());
			if(!analysisElement)
				continue;
			
			int result = analysisElement->attachToLinkedAsset(linkedTaskMatches);	//Linked Task needs to be stored before we call this
			switch(result)
			{
			case AnalysisElement::SUCCESS_BY_LINK:
				break;
			case AnalysisElement::SUCCESS_BY_PATH:
				pathMatchedAnalysisAssets.append(analysisElement->getInfoString());
				break;
			case AnalysisElement::FAIL:
				unmatchedAnalysisAssets.append(analysisElement->getInfoString());
				child->setDeleted();
				break;
			}
		}
	}
	if(pathMatchedAnalysisAssets.size() || unmatchedAnalysisAssets.size())
	{
		if(linkedTaskMatches)
			feedback = "The task design appears to have been altered outside of the Picto Development Environment.  ";
		else
			feedback = "This Analysis was built for a task design that does not appear in this experiment.  ";
		if(unmatchedAnalysisAssets.isEmpty())
		{
			feedback += "The analysis was succesfully attached to this experiment's " 
				+ task->getName().toLatin1() 
				+ " Task; however, you should verify that your analysis "
					"scripts are compatible with it.";
		}
		else
		{
			feedback += "Experimental element links could not be found for the following Analysis elements which have been removed:\n" + unmatchedAnalysisAssets.join("\n") + "\n";
		}
		if(linkedTaskMatches && pathMatchedAnalysisAssets.size())
		{
			feedback += "The following Analysis elements were attached to Experimental elements by matching name alone:\n"
				+ unmatchedAnalysisAssets.join("\n") + "\n";
		}

	}

	if(feedback.size())
		return false;
	return true;
}

void Analysis::postDeserialize()
{
	ScriptableContainer::postDeserialize();
	if(getAnalysisId() == QUuid())
		propertyContainer_->setPropertyValue("AnalysisId",QUuid::createUuid());
}

bool Analysis::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ScriptableContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

void Analysis::setLinkedTask(QSharedPointer<Task> task)
{
	Q_ASSERT(task);
	if(task == linkedTask_)
		return;
	//If this analysis was already linked to a task, disconnect that task's taskId and taskName property signals
	if(linkedTask_)
	{
		QSharedPointer<Property> taskIdProp = task->getPropertyContainer()->getProperty("TaskId");
		QSharedPointer<Property> taskNameProp = task->getPropertyContainer()->getProperty("Name");
		Q_ASSERT(taskIdProp && taskNameProp);
		disconnect(taskIdProp.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(taskPropertyEdited(Property*,QVariant)));
		disconnect(taskNameProp.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(taskPropertyEdited(Property*,QVariant)));		
	}
	linkedTask_ = task;
	updateLinkedTaskProperties();

	//Connect to change signals from task's TaskId and Name properties, so that we can update when
	//they change
	QSharedPointer<Property> taskIdProp = task->getPropertyContainer()->getProperty("TaskId");
	QSharedPointer<Property> taskNameProp = task->getPropertyContainer()->getProperty("Name");
	Q_ASSERT(taskIdProp && taskNameProp);

	connect(taskIdProp.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(taskPropertyEdited(Property*,QVariant)));
	connect(taskNameProp.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(taskPropertyEdited(Property*,QVariant)));
}

void Analysis::updateLinkedTaskProperties()
{
	propertyContainer_->setPropertyValue("LinkedTaskId",linkedTask_->getTaskId());
	propertyContainer_->setPropertyValue("LinkedTaskName",linkedTask_->getName());
}

void Analysis::taskPropertyEdited(Property*,QVariant)
{
	updateLinkedTaskProperties();
}

}; //namespace Picto
