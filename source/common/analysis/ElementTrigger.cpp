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

QSharedPointer<AnalysisDataIterator> ElementTrigger::createDataIterator()
{
	QSharedPointer<TransitionDataIterator> returnVal = QSharedPointer<TransitionDataIterator>(
							new TransitionDataIterator(qsEngine_,session_));
	returnVal->registerTransitionsByNode(
				propertyContainer_->getPropertyValue("ElementPath").toString(),
				boundaryList_.value(propertyContainer_->getPropertyValue("When").toInt(),"") == "Ends"
				);
	return returnVal;
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