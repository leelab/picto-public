#ifndef _REWARD_H_
#define _REWARD_H_

#include "../common.h"
#include "StateMachineElement.h"
#include "../engine/PictoEngine.h"

namespace Picto {

/*!	\brief A Reward is a state machine element that determines what is returned
 *	
 *	A reward is typically the final stop of a state machine. If a reward is added to a StateMachine
 *	it acts as a StateMachineElement; you can transition to it, and call run on it.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API Reward : public StateMachineElement
#else
class Reward : public StateMachineElement
#endif
{
	Q_OBJECT
	Q_PROPERTY(int number READ getNumber WRITE setNumber)
	Q_PROPERTY(int unitQuantity READ getUnitQuantity WRITE setUnitQuantity)
public:
	Reward();
	virtual ~Reward(){};
	static QSharedPointer<Asset> Create();

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	QString runAsSlave(QSharedPointer<Engine::PictoEngine> engine);
	virtual QString getUITemplate(){return "Reward";};
	virtual QString assetType(){return "Reward";};

	int getNumber(){return propertyContainer_->getPropertyValue("NumRewards").toInt();};
	void setNumber(int num){propertyContainer_->setPropertyValue("NumRewards",num);};
	int getUnitQuantity(){return propertyContainer_->getPropertyValue("RewardQty").toInt();};
	void setUnitQuantity(int quant){propertyContainer_->setPropertyValue("RewardQty",quant);};
	//! \TODO Add rewarding options here...

	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
