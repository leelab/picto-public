#include <QCoreApplication>
#include "StateMachineElement.h"

#include "../property/Property.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"
#include "../storage/StateDataUnit.h"
#include "../storage/AssetFactory.h"
#include "../memleakdetect.h"

namespace Picto {

	QString StateMachineElement::lastTransitionTime_;

StateMachineElement::StateMachineElement()
{
	//DefinePlaceholderTag("Results");
	//AddDefinableObjectFactory("Result",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(Result::Create))) );
}

QPoint StateMachineElement::getDisplayLayoutPosition()
{
	return layoutPosition_;
}

QString StateMachineElement::type()
{
	QString typeStr = propertyContainer_->getPropertyValue("Type").toString();
	return typeStr;
}

void StateMachineElement::postDeserialize()
{
	ResultContainer::postDeserialize();
}

bool StateMachineElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ResultContainer::validateObject(xmlStreamReader))
		return false;
	QString resultName;
	QStringList results = getResultList();
	foreach(QString result,results)
	{
		if(result == "EngineAbort")
		{
			addError("StateMachineElement", "EngineAbort is a resticted keyword, and may not be used as the name of a result", xmlStreamReader);
			return false;
		}
	}
	return true;
}

}; //namespace Picto
