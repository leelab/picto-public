#include "Reward.h"
#include "../engine/PictoEngine.h"
#include <QCoreApplication>
namespace Picto
{

Reward::Reward()
{
	AddDefinableProperty("Name","");
	AddDefinableProperty("Type","Reward");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	AddDefinableProperty(QVariant::Int,"RewardQty",0);
	AddDefinableProperty(QVariant::Int,"RewardChan",0);
	addResult("done");
}

QString Reward::run(QSharedPointer<Engine::PictoEngine> engine)
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

	return "done";
}

QString Reward::runAsSlave(QSharedPointer<Engine::PictoEngine> engine)
{
	QString result;// = run(engine);
	QString masterResult;
	while(masterResult.isEmpty())
	{
		masterResult = getMasterStateResult(engine);
		QCoreApplication::processEvents();
	}
	//Q_ASSERT_X(masterResult == result,"Reward::runAsSlave","Reward result was not equal to master's reward result.");
	result = masterResult;
	return result; 
}

/*!	\Brief Turns a reward object into an XML framgent.
 *
 *	The XML fragment for a reward will look like this:
 * 
 *	<StateMachineElement type="Reward">
 *		<Name>BrokeFixation</Name>
 *		<Reward quantity="3" channel="1"/>
 *	</StateMachineElement>
 */
//bool Reward::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("StateMachineElement");
//	xmlStreamWriter->writeAttribute("type","Reward");
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
//bool Reward::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateMachineElement")
//	{
//		addError("Reward","Incorrect tag, expected <StateMachineElement>",xmlStreamReader);
//		return false;
//	}
//	if(xmlStreamReader->attributes().value("type").toString() != type())
//	{
//		addError("Reward","Incorrect type of StateMachineElement, expected Reward",xmlStreamReader);
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
//				addError("Reward", "EngineAbort is a resticted keyword, and may not be used as the name of a reward", xmlStreamReader);
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
//				addError("Reward","Reward quantity not an integer",xmlStreamReader);
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
//				addError("Reward","Reward channel not an integer",xmlStreamReader);
//				return false;
//			}
//			propertyContainer_->setPropertyValue("RewardChan",QVariant(rewardQuantity));
//		}
//		else
//		{
//			addError("Reward", "Unexpected tag", xmlStreamReader);
//			return false;
//		}
//		xmlStreamReader->readNext();
//	}
//	
//	if(xmlStreamReader->atEnd())
//	{
//		addError("Reward", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//
//	return true;
//}

bool Reward::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(propertyContainer_->getPropertyValue("Name").toString() == "EngineAbort")
	{
		addError("Reward", "EngineAbort is a resticted keyword, and may not be used as the name of a reward", xmlStreamReader);
		return false;
	}

	if(propertyContainer_->getPropertyValue("RewardQty").toInt() < 0)
	{
		addError("Reward", "RewardQty must have a value greater than or equal to zero", xmlStreamReader);
		return false;
	}

	if(propertyContainer_->getPropertyValue("RewardChan").toInt() < 0)
	{
		addError("Reward", "RewardChan must have a value greater than or equal to zero", xmlStreamReader);
		return false;
	}
	return true;
}

}//namespace Picto