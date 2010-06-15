#include "Result.h"
#include "../engine/PictoEngine.h"
#include <QtDebug>
namespace Picto
{

Result::Result()
{
	propertyContainer_.setPropertyValue("Type","Result");
	propertyContainer_.addProperty(Property(QVariant::Bool,"GiveReward",false));
	propertyContainer_.addProperty(Property(QVariant::Int,"RewardQty",0));
	propertyContainer_.addProperty(Property(QVariant::Int,"RewardChan",1));
}

QString Result::run(QSharedPointer<Engine::PictoEngine> engine)
{
	if(propertyContainer_.getPropertyValue("GiveReward").toBool())
	{
		int numRewards = propertyContainer_.getPropertyValue("RewardQty").toInt();
		int rewardChan = propertyContainer_.getPropertyValue("RewardChan").toInt();
		for(int i=0; i<numRewards; i++)
		{
			engine->giveReward(rewardChan);
		}
	}
	return propertyContainer_.getPropertyValue("Name").toString();
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
bool Result::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("StateMachineElement");
	xmlStreamWriter->writeAttribute("type","Result");
	
	xmlStreamWriter->writeTextElement("Name", propertyContainer_.getPropertyValue("Name").toString());

	if(propertyContainer_.getPropertyValue("GiveReward").toBool())
	{
		xmlStreamWriter->writeStartElement("Reward");
		xmlStreamWriter->writeAttribute("quantity", propertyContainer_.getPropertyValue("RewardQty").toString());
		xmlStreamWriter->writeAttribute("channel", propertyContainer_.getPropertyValue("RewardChan").toString());
		xmlStreamWriter->writeEndElement();
	}

	xmlStreamWriter->writeEndElement(); //StateMachineElement
	return true;
}


bool Result::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateMachineElement")
	{
		addError("Result","Incorrect tag, expected <StateMachineElement>",xmlStreamReader);
		return false;
	}
	if(xmlStreamReader->attributes().value("type").toString() != type())
	{
		addError("Result","Incorrect type of StateMachineElement, expected Result",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateMachineElement") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Name")
		{
			QString elementText = xmlStreamReader->readElementText();
			if(elementText == "EngineAbort")
			{
				addError("Result", "EngineAbort is a resticted keyword, and may not be used as the name of a result", xmlStreamReader);
				return false;
			}
			propertyContainer_.setPropertyValue("Name",QVariant(elementText));
		}
		else if(name == "Reward")
		{
			propertyContainer_.setPropertyValue("GiveReward",QVariant(true));
			bool ok;
			int rewardQuantity = xmlStreamReader->attributes().value("quantity").toString().toInt(&ok);
			if(!ok)
			{
				addError("Result","Reward quantity attribute not an integer",xmlStreamReader);
				return false;
			}
			propertyContainer_.setPropertyValue("RewardQty",QVariant(rewardQuantity));

			int rewardChan = xmlStreamReader->attributes().value("channel").toString().toInt(&ok);
			if(!ok)
			{
				addError("Result","Reward channel attribute not an integer",xmlStreamReader);
				return false;
			}
			propertyContainer_.setPropertyValue("RewardChan",QVariant(rewardChan));
		}
		else
		{
			addError("Result", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}
	
	if(xmlStreamReader->atEnd())
	{
		addError("Result", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	return true;
}
}//namespace Picto