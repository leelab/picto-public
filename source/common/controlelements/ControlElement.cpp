#include "ControlElement.h"

namespace Picto {

ControlElement::ControlElement()
{
}

//bool ControlElement::addResult(QSharedPointer<Result> result)
//{
//	results_[result->name()] = result;
//	QList<QSharedPointer<Asset>> resultChildren = getGeneratedChildren("Result");
//	bool found = false;
//	foreach(QSharedPointer<Asset> resultChild,resultChildren)
//	{
//		if(resultChild->name() == result->name())
//		{
//			found = true;
//			break;
//		}
//	}
//	if(!found)
//		AddChild("Result",result);
//	return true;
//}
//
//QSharedPointer<Result> ControlElement::getResult(QString name)
//{
//	if(results_.contains(name))
//		return results_[name];
//	return QSharedPointer<Result>();
//}

void ControlElement::postDeserialize()
{
	ResultContainer::postDeserialize();
	////Re-Add all default results as children since serialization clears the child list.
	//QStringList resultKeys = results_.keys();
	//foreach(QString resultKey,resultKeys)
	//{
	//	AddChild("Result",results_[resultKey]);
	//}
}

bool ControlElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ResultContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
