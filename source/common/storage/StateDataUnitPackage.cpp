#include "StateDataUnitPackage.h"
#include "../memleakdetect.h"

namespace Picto {

StateDataUnitPackage::StateDataUnitPackage()
{
}
/*! \brief Adds the input Transition to this package's list of Transition traversals.
*/
void StateDataUnitPackage::addTransition(QSharedPointer<Transition> transition)
{
	QSharedPointer<StateDataUnit> data(new StateDataUnit());
	data->setTransition(transition);
	data_.append(data);
}

/*! \brief Adds a Transition by its AssetId to this package's list of Transition traversals.
*/
void StateDataUnitPackage::addTransition(int id)
{
	QSharedPointer<StateDataUnit> data(new StateDataUnit());
	data->setTransition(id);
	data_.append(data);
}

/*! \brief Sets the DataId of the Frame that was presented following all Transition traversals
 *	stored in this package.
 */
void StateDataUnitPackage::setActionFrame(qulonglong frameId)
{
	for(QList<QSharedPointer<StateDataUnit>>::iterator it = data_.begin();it != data_.end();it++)
	{
		(*it)->setActionFrame(frameId);
	}
}

/*! \brief Turns the StateDataUnitPackage into an XML fragment
 */
bool StateDataUnitPackage::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("StateDataUnitPackage");

	foreach(QSharedPointer<StateDataUnit> data, data_)
	{
		data->toXml(xmlStreamWriter);
	}

	xmlStreamWriter->writeEndElement();

	return true;
}
/*! Converts XML into a StateDataUnitPackage object.  Note that this deletes any existing data. */
bool StateDataUnitPackage::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	clear();

	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateDataUnitPackage")
	{
		addError("Incorrect tag, expected <StateDataUnitPackage>");
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
		if(name == "SDU")
		{
			QSharedPointer<StateDataUnit> data(new StateDataUnit());
			data->fromXml(xmlStreamReader);
			data_.append(data);
		}
		else
		{
			addError("Unexpected tag");
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("Unexpected end of document");
		return false;
	}
	return true;
}

}; //namespace Picto
