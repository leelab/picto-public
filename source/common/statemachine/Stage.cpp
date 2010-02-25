#include "Stage.h"

namespace Picto {

Stage::Stage()
{
	type_ = "Stage";

	//Stages can contain states, results, flow elements, and 
	//other stages
	validConatinedTypes_.push_back("State");
	validConatinedTypes_.push_back("Stage");
	validConatinedTypes_.push_back("Result");
	validConatinedTypes_.push_back("FlowElement");
}

QString Stage::run()
{
	QUuid currId;
	QUuid nextId;
	QSharedPointer<StateMachineElement> currElement;
	QSharedPointer<StateMachineElement> nextElement;

	currId = initialElement_;
	
	while(true)
	{
		currElement = PictoIdentifiableObject::getObjectByID(currId).staticCast<StateMachineElement>();
		QString result = currElement->run();

		if(transitions_.contains(currId) && transitions_.value(currId).contains(result))
			nextId = transitions_.value(currId).value(result);
		else
			nextId = defaultTransitions_.value(currId);

		nextElement = PictoIdentifiableObject::getObjectByID(nextId).staticCast<StateMachineElement>();
		Q_ASSERT(nextElement);
		if(nextElement->type() == "Result")
			return nextElement->run();
		else
			currElement = nextElement;
	}
}

}; //namespace Picto
