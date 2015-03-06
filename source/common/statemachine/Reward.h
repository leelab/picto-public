#ifndef _REWARD_H_
#define _REWARD_H_

#include "../common.h"
#include "StateMachineElement.h"
#include "../engine/PictoEngine.h"

namespace Picto {

/*!	\brief A StateMachineElement that initiates reward feedback for the test subject.
 *	
 *	\details The Reward element provides a number of Properties to define the parameters of the reward
 *	that will be provided when it is reached.  These include the number of rewards, their duration and 
 *	the spacing between them.  It is important to note, however, that no experimental time passes inside
 *	a Reward element.  When the element is reached it initiates the reward supply process, but control 
 *	returns immediately and continues on to the next element.  This means that the design needs to define
 *	what the test subject is seeing during the course of rewarding.  If, for example, the Reward element
 *	is set to provide 2 rewards of 100ms each with a minimum period of 300ms.  As soon as control flow 
 *	reaches the Reward element, the PictoEngine will be set to start a reward at the onset of the following
 *	frame, that reward will end 100ms from the onset of that frame, another reward will be provided
 *	at the first frame onset following 300ms after the onset of that first frame and it will end 100ms after 
 *	that.  This whole process will take something like 400ms, so the design will need to include State elements that
 *	handle the display during the course of that reward process.  This is useful for creating animations
 *	representing reward delivery like reward tokens that slowly disappear during the course of the reward
 *	supply.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API Reward : public StateMachineElement
#else
class Reward : public StateMachineElement
#endif
{
	Q_OBJECT
	/*! \brief Gets/Sets the number of rewards that will be provided when this Reward is triggered.*/
	Q_PROPERTY(int number READ getNumber WRITE setNumber)
	/*! \brief Gets/Sets the number of milliseconds for which each reward will be provided.*/
	Q_PROPERTY(int unitQuantity READ getUnitQuantity WRITE setUnitQuantity)
	/*! \brief Gets/Sets the minimum number of milliseconds between rewards when multiple rewards are provided.*/
	Q_PROPERTY(int minRewardPeriod READ getMinRewardPeriod WRITE setMinRewardPeriod)
public:
	Reward();
	virtual ~Reward(){};
	static QSharedPointer<Asset> Create();

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	QString slaveRun(QSharedPointer<Engine::PictoEngine> engine);
	virtual QString getUITemplate(){return "Reward";};
	virtual QString friendlyTypeName(){return "Reward";};

	/*! \brief Gets the number of rewards that will be provided when this Reward is triggered.*/
	int getNumber(){return propertyContainer_->getPropertyValue("NumRewards").toInt();};
	/*! \brief Sets the number of rewards that will be provided when this Reward is triggered.*/
	void setNumber(int num){propertyContainer_->setPropertyValue("NumRewards",num);};
	/*! \brief Gets the number of milliseconds for which each reward will be provided.*/
	int getUnitQuantity(){return propertyContainer_->getPropertyValue("RewardQty").toInt();};
	/*! \brief Sets the number of milliseconds for which each reward will be provided.*/
	void setUnitQuantity(int quant){propertyContainer_->setPropertyValue("RewardQty",quant);};
	/*! \brief Gets the minimum number of milliseconds between rewards when multiple rewards are provided.*/
	int getMinRewardPeriod(){return propertyContainer_->getPropertyValue("MinRewardPeriod").toInt();};
	/*! \brief Sets the minimum number of milliseconds between rewards when multiple rewards are provided.*/	
	void setMinRewardPeriod(int delay){propertyContainer_->setPropertyValue("MinRewardPeriod",delay);};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
