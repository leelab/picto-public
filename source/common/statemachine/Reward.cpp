#include <QCoreApplication>
#include "Reward.h"
#include "../engine/PictoEngine.h"
#include "../memleakdetect.h"
namespace Picto
{

Reward::Reward()
{
	setMaxOptionalResults(0);
	parameterFactory_->setMaxAssets(0);
	visualElementFactory_->setMaxAssets(0);
	controlTargetFactory_->setMaxAssets(0);
	AddDefinableProperty("Type","Reward");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	AddDefinableProperty(QVariant::Int,"NumRewards",1);
	AddDefinableProperty(QVariant::Int,"RewardQty",60);
	AddDefinableProperty(QVariant::Int,"MinRewardPeriod",125);
	AddDefinableProperty(QVariant::Int,"RewardChan",1);
	addRequiredResult("done");
}

QSharedPointer<Asset> Reward::Create()
{
	return QSharedPointer<Asset>(new Reward());
}

QString Reward::run(QSharedPointer<Engine::PictoEngine> engine)
{
	resetScriptableValues();
	int numRewards = propertyContainer_->getPropertyValue("NumRewards").toInt();
	int rewardQty = propertyContainer_->getPropertyValue("RewardQty").toInt();
	int rewardChan = propertyContainer_->getPropertyValue("RewardChan").toInt();
	int minRewardPeriod = propertyContainer_->getPropertyValue("MinRewardPeriod").toInt();

	//Give the rewards
	if(numRewards > 0)
	{
		for(int i=0; i<numRewards; i++)
		{
			engine->giveReward(rewardChan,rewardQty,minRewardPeriod);
		}
	}

	return "done";
}

QString Reward::slaveRun(QSharedPointer<Engine::PictoEngine> engine)
{
	QString result;
	return result; 
}

void Reward::postDeserialize()
{
	StateMachineElement::postDeserialize();
	setPropertyRuntimeEditable("NumRewards");
	setPropertyRuntimeEditable("RewardQty");
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

	if(propertyContainer_->getPropertyValue("NumRewards").toInt() < 0)
	{
		addError("Reward", "NumRewards must have a value greater than or equal to zero", xmlStreamReader);
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