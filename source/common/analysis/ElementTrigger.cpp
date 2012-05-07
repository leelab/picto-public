#include <QSqlQuery>
#include "ElementTrigger.h"
using namespace Picto;

ElementTrigger::ElementTrigger()
{
	AddDefinableProperty("ElementPath","");
	boundaryList_ << "Starts" << "Ends";
	AddDefinableProperty(QtVariantPropertyManager::enumTypeId(),"When",0,"enumNames",boundaryList_);

}

ElementTrigger::~ElementTrigger()
{

}

QSharedPointer<Asset> ElementTrigger::Create()
{
	return QSharedPointer<Asset>(new ElementTrigger());
}

EventOrderIndex ElementTrigger::getNextTriggerTime()
{
	if(!transIterator_)
	{
		transIterator_ = QSharedPointer<TransitionDataIterator>(
							new TransitionDataIterator(qsEngine_,session_));
		transIterator_->registerTransitionsByNode(
						propertyContainer_->getPropertyValue("ElementPath").toString(),
						boundaryList_.value(propertyContainer_->getPropertyValue("When").toInt(),"") == "Ends"
						);
	}
	return transIterator_->getNextValue()->index;
}

void ElementTrigger::restart()
{
	transIterator_.clear();
}

float ElementTrigger::fractionTriggersRemaining()
{
	if(!transIterator_) return 1.0;
	return transIterator_->fractionRemaining();
}

void ElementTrigger::recheckSessionData()
{
	if(!transIterator_)
		return;
	transIterator_->recheckSessionData();
}

void ElementTrigger::postDeserialize()
{
	AnalysisTrigger::postDeserialize();
}

bool ElementTrigger::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTrigger::validateObject(xmlStreamReader))
		return false;
	return true;
}