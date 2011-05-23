#include "Result.h"
#include "../engine/PictoEngine.h"
#include <QCoreApplication>
namespace Picto
{

Result::Result()
{
	AddDefinableProperty("Name","");
	AddDefinableProperty("Type","Result");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	AddDefinableProperty(QVariant::Int,"RewardQty",0);
	AddDefinableProperty(QVariant::Int,"RewardChan",0);
}

QString Result::run(QSharedPointer<Engine::PictoEngine> engine)
{

	int numRewards = propertyContainer_->getPropertyValue("RewardQty").toInt();
	int rewardChan = propertyContainer_->getPropertyValue("RewardChan").toInt();

	//Give the rewards
	if(numRewards > 0)
	{
		for(int i=0; i<numRewards; i++)
		{
			engine->giveReward(rewardChan);
		}
	}

	return propertyContainer_->getPropertyValue("Name").toString();
}

QString Result::runAsSlave(QSharedPointer<Engine::PictoEngine> engine)
{
	return run(engine);
}

/*!	\Brief Turns a result object into an XML framgent.
 *
 *	The XML fragment for a result will look like this:
 * 
 *	<StateMachineElement type="Result">
 *		<Name>BrokeFixation</Name>
 *		<Reward quantity="3" channel="1"/>
 *	</StateMachineElement>
 */
//bool Result::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("StateMachineElement");
//	xmlStreamWriter->writeAttribute("type","Result");
//	
//	xmlStreamWriter->writeTextElement("Name", propertyContainer_->getPropertyValue("Name").toString());
//	xmlStreamWriter->writeTextElement("RewardQty", propertyContainer_->getPropertyValue("RewardQty").toString());
//	xmlStreamWriter->writeTextElement("RewardChan", propertyContainer_->getPropertyValue("RewardChan").toString());
//
//	xmlStreamWriter->writeEndElement(); //StateMachineElement
//	return true;
//}
//
//
//bool Result::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateMachineElement")
//	{
//		addError("Result","Incorrect tag, expected <StateMachineElement>",xmlStreamReader);
//		return false;
//	}
//	if(xmlStreamReader->attributes().value("type").toString() != type())
//	{
//		addError("Result","Incorrect type of StateMachineElement, expected Result",xmlStreamReader);
//		return false;
//	}
//
//	xmlStreamReader->readNext();
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateMachineElement") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->name().toString();
//		if(name == "Name")
//		{
//			QString elementText = xmlStreamReader->readElementText();
//			if(elementText == "EngineAbort")
//			{
//				addError("Result", "EngineAbort is a resticted keyword, and may not be used as the name of a result", xmlStreamReader);
//				return false;
//			}
//			propertyContainer_->setPropertyValue("Name",QVariant(elementText));
//		}
//		else if(name == "RewardQty")
//		{
//			QString elementText = xmlStreamReader->readElementText();
//			bool ok;
//			int rewardQuantity = elementText.toInt(&ok);
//			if(!ok)
//			{
//				addError("Result","Reward quantity not an integer",xmlStreamReader);
//				return false;
//			}
//			propertyContainer_->setPropertyValue("RewardQty",QVariant(rewardQuantity));
//		}
//		else if(name == "RewardChan")
//		{
//			QString elementText = xmlStreamReader->readElementText();
//			bool ok;
//			int rewardQuantity = elementText.toInt(&ok);
//			if(!ok)
//			{
//				addError("Result","Reward channel not an integer",xmlStreamReader);
//				return false;
//			}
//			propertyContainer_->setPropertyValue("RewardChan",QVariant(rewardQuantity));
//		}
//		else
//		{
//			addError("Result", "Unexpected tag", xmlStreamReader);
//			return false;
//		}
//		xmlStreamReader->readNext();
//	}
//	
//	if(xmlStreamReader->atEnd())
//	{
//		addError("Result", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//
//	return true;
//}

bool Result::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(propertyContainer_->getPropertyValue("Name").toString() == "EngineAbort")
	{
		addError("Result", "EngineAbort is a resticted keyword, and may not be used as the name of a result", xmlStreamReader);
		return false;
	}

	if(propertyContainer_->getPropertyValue("RewardQty").toInt() < 0)
	{
		addError("Result", "RewardQty must have a value greater than or equal to zero", xmlStreamReader);
		return false;
	}

	if(propertyContainer_->getPropertyValue("RewardChan").toInt() < 0)
	{
		addError("Result", "RewardChan must have a value greater than or equal to zero", xmlStreamReader);
		return false;
	}
	return true;
}

}//namespace Picto