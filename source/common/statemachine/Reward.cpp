#include <QCoreApplication>
#include "Reward.h"
#include "LogicResult.h"
#include "../engine/PictoEngine.h"
#include "../memleakdetect.h"
namespace Picto
{

/*! \brief Constructs a new Reward object.
 *	\details Adds the following Properties: 
 *		- NumRewards - Editable at runtime: defines the number of rewards that will be provided each time control flow reaches this element.
 *		- RewardQty - Editable at runtime: The number of milliseconds for which reward will be provided for each provided reward.
 *		- MinRewardPeriod: The minimum number of milliseconds between the start of subsequent rewards.
 *		- RewardChan: The reward channel on which the reward will be provided (Not currently visible since Pictoboxes currently only have one reward channel).
 */
Reward::Reward()
{
	setMaxOptionalResults(0);
	AddDefinableProperty(QVariant::Int,"NumRewards",1);
	AddDefinableProperty(QVariant::Int,"RewardQty",60);
	AddDefinableProperty(QVariant::Int,"MinRewardPeriod",125);
	AddDefinableProperty(QVariant::Int,"RewardChan",1);
	defineResultFactoryType("",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LogicResult::Create))));
	addRequiredResult("done");
}

/*! \brief Creates a new Reward object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> Reward::Create()
{
	return QSharedPointer<Asset>(new Reward());
}

/*! \brief Runs this Reward's execution logic within the framework of the input PictoEngine.
 *	\details This function handles running entry and exit scripts, and initializing reward delivery according to its Property values.
 *	No experimental time passes during this function since Picto provides Rewards asynchoronously.  This is important to keep in mind
 *	in experimental designs.  When a Reward with NumRewards of 1 and RewardQty equal to 100 is triggered, for example, a reward will
 *	begin at the next first phosphore time and will continue for 100ms thereafter.  This means that it is up to the Design to manage
 *	what is visible to the test subject while a reward is being provided.
*/
QString Reward::run(QSharedPointer<Engine::PictoEngine> engine)
{
	resetScriptableValues();

	runEntryScript();
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
	setLatestResult("done");
	getResult("done")->runResultScript();
	runExitScript();
	return "done";
}

/*! \brief When a Reward is run as a slave, it really doesn't do anything.  The SlaveExperimentDriver tells the PictoEngine when to trigger an audio reward
 *	based on the reward timing data in the session file.  This function is therefore empty.
 */
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
	setPropertyRuntimeEditable("MinRewardPeriod");	
}

/*! \brief Extends StateMachineElement::validateObject() to verify that Property values for this element are greater than zero.
*/
bool Reward::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!StateMachineElement::validateObject(xmlStreamReader))
		return false;

	if(propertyContainer_->getPropertyValue("Name").toString() == "EngineAbort")
	{
		addError("EngineAbort is a resticted keyword, and may not be used as the name of a reward");
		return false;
	}

	if(propertyContainer_->getPropertyValue("NumRewards").toInt() < 0)
	{
		addError("NumRewards must have a value greater than or equal to zero");
		return false;
	}

	if(propertyContainer_->getPropertyValue("RewardQty").toInt() < 0)
	{
		addError("RewardQty must have a value greater than or equal to zero");
		return false;
	}

	if(propertyContainer_->getPropertyValue("RewardChan").toInt() < 0)
	{
		addError("RewardChan must have a value greater than or equal to zero");
		return false;
	}
	return true;
}

}//namespace Picto