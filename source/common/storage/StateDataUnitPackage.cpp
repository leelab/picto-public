#include "StateDataUnitPackage.h"

namespace Picto {

StateDataUnitPackage::StateDataUnitPackage()
{
}

void StateDataUnitPackage::addTransition(QSharedPointer<Transition> transition, double timestamp, QString stateMachinePath)
{
	QSharedPointer<StateDataUnit> data(new StateDataUnit());
	data->setTransition(transition,timestamp,stateMachinePath);
	data_.append(data);
}

void StateDataUnitPackage::addTransition(QString source, QString sourceResult, QString destination, double timestamp, int id, QString stateMachinePath)
{
	QSharedPointer<StateDataUnit> data(new StateDataUnit());
	data->setTransition(source,sourceResult,destination,timestamp,id,stateMachinePath);
	data_.append(data);
}

void StateDataUnitPackage::addTransition(QString source, QString sourceResult, QString destination, QString timestamp, int id, QString stateMachinePath)
{
	QSharedPointer<StateDataUnit> data(new StateDataUnit());
	data->setTransition(source,sourceResult,destination,timestamp,id,stateMachinePath);
	data_.append(data);
}

void StateDataUnitPackage::setAllTimestamps(double newTime)
{
	QString timeStr = QString("%1").arg(newTime,0,'f',6);
	for(QList<QSharedPointer<StateDataUnit>>::iterator it = data_.begin();it != data_.end();it++)
	{
		(*it)->setTime(timeStr);
	}
}

/*! \brief Turns the StateDataUnitPackage into an XML fragment
 *
 *	The XML will look like this:
 *	<StateDataUnitPackage>
 *		<Frame timestamp=1.234 state=somestate>1</Frame>
 *		<Frame timestamp=1.434 state=somestate>2</Frame>
 *		...
 *	</StateDataUnitPackage>
 *	
 */
bool StateDataUnitPackage::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("StateDataUnitPackage");

	foreach(QSharedPointer<StateDataUnit> data, data_)
	{
		data->toXml(xmlStreamWriter);
	}

	xmlStreamWriter->writeEndElement(); //BehavioralDataUnitPackage

	return true;
}
//! Converts XML into a StateDataUnitPackage object.  Note that this deletes any existing data.
bool StateDataUnitPackage::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	clear();

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateDataUnitPackage")
	{
		addError("StateDataUnitPackage","Incorrect tag, expected <StateDataUnitPackage>",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateDataUnitPackage") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "StateDataUnit")
		{
			QSharedPointer<StateDataUnit> data(new StateDataUnit());
			data->fromXml(xmlStreamReader);
			data_.append(data);
		}
		else
		{
			addError("StateDataUnitPackage", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("StateDataUnitPackage", "Unexpected end of document", xmlStreamReader);
		return false;
	}
	return true;
}

void StateDataUnitPackage::postDeserialize()
{

}

bool StateDataUnitPackage::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	return true;
}


}; //namespace Picto
