#include "DirectorData.h"
#include "../../common/memleakdetect.h"

namespace Picto {


DirectorData::DirectorData()
{
}

/*! \brief Turns the DirectorDataPackage into an XML fragment
 *
 *	
 */
bool DirectorData::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("DD");
	foreach(QSharedPointer<RewardUnit> rewardUn,rewardMap_)
	{
		rewardUn->serializeAsXml(xmlStreamWriter);
	}
	xmlStreamWriter->writeEndElement();
	return true;
}
//! Converts XML into a DirectorDataPackage object.  Note that this deletes any existing data.
bool DirectorData::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	rewardMap_.clear();
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "DD")
	{
		addError("DirectorData","Incorrect tag, expected <DD>",xmlStreamReader);
		return false;
	}

	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "DD") && !xmlStreamReader->atEnd())
	{
		xmlStreamReader->readNext();
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "RU")
		{
			QSharedPointer<RewardUnit> ru(new RewardUnit());
			ru->deserializeFromXml(xmlStreamReader);
			rewardMap_[ru->getChannel()] = ru;
		}
	}
	return true;
}

void DirectorData::setReward(int channel, int duration)
{
	QSharedPointer<RewardUnit> ru;
	if(rewardMap_.contains(channel))
	{
		ru = rewardMap_.value(channel);
		ru->setDuration(duration);
		return;
	}
	ru = QSharedPointer<RewardUnit>(new RewardUnit(channel,duration));
	rewardMap_[channel] = ru;
}

int DirectorData::getRewardDuration(int channel)
{
	if(rewardMap_.contains(channel))
		return rewardMap_.value(channel)->getDuration();
	return -1;
}

QHash<int,int> DirectorData::getRewardMap()
{
	QHash<int,int> returnVal;
	foreach(int key,rewardMap_.keys())
	{
		returnVal[key] = getRewardDuration(key);
	}
	return returnVal;
}

void DirectorData::postDeserialize()
{

}

bool DirectorData::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
