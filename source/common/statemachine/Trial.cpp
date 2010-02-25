#include "Trial.h"

namespace Picto {

Trial::Trial()
{
	type_ = "Trial";

	//Trials can contain stages, results, and flow elements
	validConatinedTypes_.push_back("Stage");
	validConatinedTypes_.push_back("Result");
	validConatinedTypes_.push_back("FlowElement");
}

QString Trial::run()
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
