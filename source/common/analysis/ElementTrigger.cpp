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
							new TransitionDataIterator(session_));
		transIterator_->registerTransitionsByNode(
						propertyContainer_->getPropertyValue("ElementPath").toString(),
						boundaryList_.value(propertyContainer_->getPropertyValue("When").toInt(),"") == "Ends"
						);
		Q_ASSERT(transIterator_->isValid());
	}
	return transIterator_->getNextTransitionTraversal();
}

void ElementTrigger::restart()
{
	transIterator_.clear();
}

unsigned int ElementTrigger::totalKnownTriggers()
{
	return transIterator_->totalValues();
}

unsigned int ElementTrigger::remainingKnownTriggers()
{
	return transIterator_->remainingValues();
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