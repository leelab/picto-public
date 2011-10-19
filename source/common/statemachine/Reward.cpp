#include "Reward.h"
#include "../engine/PictoEngine.h"
#include <QCoreApplication>
namespace Picto
{

Reward::Reward()
{
	setMaxOptionalResults(0);
	parameterFactory_->setMaxAssets(0);
	visualElementFactory_->setMaxAssets(0);
	controlTargetFactory_->setMaxAssets(0);
	AddDefinableProperty("Type","Reward");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	AddDefinableProperty(QVariant::Int,"RewardQty",0);
	AddDefinableProperty(QVariant::Int,"RewardChan",0);
	addRequiredResult("done");
}

QString Reward::run(QSharedPointer<Engine::PictoEngine> engine)
{
	resetScriptableValues();
	int numRewards = propertyContainer_->getPropertyValue("RewardQty").toInt();
	int rewardChan = propertyContainer_->getPropertyValue("RewardChan").toInt();

	//Give the rewards
	if(numRewards > 0)
	{
		for(int i=0; i<numRewards; i++)
		{
			engine->giveReward(rewardChan);
		}
	}

	return "done";
}

QString Reward::runAsSlave(QSharedPointer<Engine::PictoEngine> engine)
{
	//resetScriptableValues();
	QString result;// = run(engine);
	QString masterResult;
	//int numRewards = propertyContainer_->getPropertyValue("RewardQty").toInt();
	//int rewardChan = propertyContainer_->getPropertyValue("RewardChan").toInt();

	//int rewardsSupplied = 0;
	//while(rewardsSupplied < numRewards)
	//{
	//	if(rewardsSupplied < numRewards)
	//	{
	//		engine->giveReward(rewardChan);
	//		rewardsSupplied++;
	//	}
	//	QCoreApplication::processEvents();
	//}
	while(masterResult.isEmpty())
	{
		engine->updateCurrentStateFromServer();
		masterResult = engine->getServerPathUpdate();
	}
	//Q_ASSERT_X(masterResult == result,"Reward::runAsSlave","Reward result was not equal to master's reward result.");
	result = masterResult;
	return result; 
}

void Reward::postSerialize()
{
	StateMachineElement::postSerialize();
}

bool Reward::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!StateMachineElement::validateObject(xmlStreamReader))
		return false;

	if(propertyContainer_->getPropertyValue("Name").toString() == "EngineAbort")
	{
		addError("Reward", "EngineAbort is a resticted keyword, and may not be used as the name of a reward", xmlStreamReader);
		return false;
	}

	if(propertyContainer_->getPropertyValue("RewardQty").toInt() < 0)
	{
		addError("Reward", "RewardQty must have a value greater than or equal to zero", xmlStreamReader);
		return false;
	}

	if(propertyContainer_->getPropertyValue("RewardChan").toInt() < 0)
	{
		addError("Reward", "RewardChan must have a value greater than or equal to zero", xmlStreamReader);
		return false;
	}
	return true;
}

}//namespace Picto