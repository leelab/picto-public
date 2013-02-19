#include "SlaveExperimentDriver.h"
#include "../memleakdetect.h"

using namespace Picto;
#define FRAMEMS 16
SlaveExperimentDriver::SlaveExperimentDriver(QSharedPointer<Experiment> exp,QSharedPointer<StateUpdater> updater)
{
	experiment_ = exp;
	updater_ = updater;
	expConfig_ = experiment_->getExperimentConfig();
	frameTimer_.restart();

	//Initialize scripting for this experiment in case this hasn't been done yet
	experiment_->initScripting(false);
	renderingEnabled_ = true;
	connect(updater_.data(),SIGNAL(propertyValueChanged(int, QString)),this,SLOT(masterPropertyValueChanged(int, QString)));
	connect(updater_.data(),SIGNAL(propertyInitValueChanged(int, QString)),this,SLOT(masterPropertyInitValueChanged(int, QString)));
	connect(updater_.data(),SIGNAL(transitionActivated(int)),this,SLOT(masterTransitionActivated(int)));
	connect(updater_.data(),SIGNAL(framePresented(double)),this,SLOT(masterFramePresented(double)));
	connect(updater_.data(),SIGNAL(rewardSupplied(double,int,int)),this,SLOT(masterRewardSupplied(double,int,int)));
	connect(updater_.data(),SIGNAL(signalChanged(QString,QStringList,QVector<float>)),this,SLOT(masterSignalChanged(QString,QStringList,QVector<float>)));
	connect(updater_.data(),SIGNAL(disableRendering(bool)),this,SLOT(disableRendering(bool)));
}

void SlaveExperimentDriver::renderFrame()
{
	if(!currElement_)
		return;
	
	if(frameTimer_.elapsed() < FRAMEMS || !renderingEnabled_)
		return;

	//Don't render the frame more than once per FRAMEMS
	currElement_->slaveRenderFrame(experiment_->getEngine());
	frameTimer_.restart();
}

void SlaveExperimentDriver::masterPropertyValueChanged(int propId, QString value)
{
	QSharedPointer<Asset> asset = expConfig_->getAsset(propId);
	Q_ASSERT(asset && asset->inherits("Picto::Property"));
	if(propId == 3329)
	{
		QString name = asset->getName();
		int i=0;
		i++;
	}
	asset.staticCast<Property>()->valFromUserString(value);
}
void SlaveExperimentDriver::masterPropertyInitValueChanged(int propId, QString value)
{
	QSharedPointer<Asset> asset = expConfig_->getAsset(propId);
	Q_ASSERT(asset && asset->inherits("Picto::Property"));
	if(propId == 3329)
	{
		QString name = asset->getName();
		int i=0;
		i++;
	}
	asset.staticCast<Property>()->initValFromUserString(value);
}
void SlaveExperimentDriver::masterTransitionActivated(int transId)
{
	QSharedPointer<Asset> asset = expConfig_->getAsset(transId);
	if(!asset || !asset->inherits("Picto::Transition"))
	{
		Q_ASSERT(!asset);
		return;
	}
	QSharedPointer<Transition> trans = asset.staticCast<Transition>();
	
	//In future, operations that occur in the transition will occur here//
	//
	//////////////////
	
	QString newResult = trans->getDestination();
	if(newResult == "EngineAbort")
	{
		return;
	}
	
	QSharedPointer<Asset> destAsset = trans->getDestinationAsset();
	if(!destAsset || !destAsset->inherits("Picto::StateMachineElement"))
	{
		Q_ASSERT(!destAsset || destAsset->inherits("Picto::Result"));
		return;
	}
	currElement_ = destAsset.staticCast<StateMachineElement>();
	currElement_->slaveRun(experiment_->getEngine());
}
void SlaveExperimentDriver::masterFramePresented(double time)
{
	renderFrame();
}
void SlaveExperimentDriver::masterRewardSupplied(double time,int duration,int channel)
{
	experiment_->getEngine()->giveReward(channel,duration,duration);
}
void SlaveExperimentDriver::masterSignalChanged(QString name,QStringList subChanNames,QVector<float> vals)
{
	int numSubChans = subChanNames.size();
	QSharedPointer<SignalChannel> sigChan = experiment_->getEngine()->getSignalChannel(name);
	if(!sigChan)
		return;
	for(int i=0;i<vals.size();i++)
	{
		sigChan->insertValue(subChanNames[i%numSubChans],vals[i]);
	}
}
void SlaveExperimentDriver::disableRendering(bool disable)
{
	bool shouldRenderFrame = !disable && !renderingEnabled_;
	renderingEnabled_=!disable;
	if(shouldRenderFrame)
		renderFrame();
}