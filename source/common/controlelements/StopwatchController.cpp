#include "StopwatchController.h"

namespace Picto
{
StopwatchController::StopwatchController()
{
	propertyContainer_->setPropertyValue("Type",ControllerType());

	QSharedPointer<Property> unitsEnumProperty = propertyContainer_->addProperty(QtVariantPropertyManager::enumTypeId(),"Units",0);
    unitList_ << "Sec" << "Ms" << "Us";
	unitsEnumProperty->addAttribute("enumNames", unitList_);

	propertyContainer_->addProperty(QVariant::Int,"Time",0);

	//Make sure to update the list of results...
	results_.append("Success");
}


ControlElement* StopwatchController::NewStopwatchController()
{
	return new StopwatchController;
}

QString StopwatchController::ControllerType()
{
	return "Stopwatch Controller";
}

void StopwatchController::setTime(int time, Controller::TimerUnits::TimerUnits units)
{
	int newUnit;

	if(units == Controller::TimerUnits::ms)
		newUnit = unitList_.indexOf("Ms");
	else if(units == Controller::TimerUnits::us)
		newUnit = unitList_.indexOf("Us");
	else if(units == Controller::TimerUnits::sec)
		newUnit = unitList_.indexOf("Sec");
	else 
		return;

	propertyContainer_->setPropertyValue("Units",newUnit);
	propertyContainer_->setPropertyValue("Time",time);

}


void StopwatchController::start(QSharedPointer<Engine::PictoEngine> engine)
{
	Q_UNUSED(engine);

	isDone_ = false;
	timer_.start();
}

bool StopwatchController::isDone(QSharedPointer<Engine::PictoEngine> engine)
{
	Q_UNUSED(engine);

	Controller::TimerUnits::TimerUnits units;
	if(unitList_.value(propertyContainer_->getPropertyValue("Units").toInt(),"") == "Sec")
		units = Controller::TimerUnits::sec;
	else if(unitList_.value(propertyContainer_->getPropertyValue("Units").toInt(),"") == "Ms")
		units = Controller::TimerUnits::ms;
	else if(unitList_.value(propertyContainer_->getPropertyValue("Units").toInt(),"") == "Us")
		units = Controller::TimerUnits::us;
	else
		Q_ASSERT(false);

	if(timer_.elapsedTime(units) > propertyContainer_->getPropertyValue("Time").toInt())
	{
		isDone_ = true;
		return true;
	}
	else
	{
		return false;
	}
}

QString StopwatchController::getResult()
{
	if(isDone_)
		return "Success";
	else
		return "";
}



/*! \brief Turns the ControlElement into an XML fragment
 *
 * A serialized StopwatchController will look something like this:
 *	<ControlElement type="StopwatchController" operatorVisible="true subjectVisible="false">
 *		<Name>MyControlElement</Name>
 *		<Time units="Ms">500</Time>
 *	</ControlElement>
 */
//
bool StopwatchController::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("ControlElement");
	xmlStreamWriter->writeAttribute("type",propertyContainer_->getPropertyValue("Type").toString());
	if(operatorVisible_)
		xmlStreamWriter->writeAttribute("operatorVisible","true");
	else
		xmlStreamWriter->writeAttribute("operatorVisible","false");
	if(subjectVisible_)
		xmlStreamWriter->writeAttribute("subjectVisible","true");
	else
		xmlStreamWriter->writeAttribute("subjectVisible","false");

	xmlStreamWriter->writeTextElement("Name", propertyContainer_->getPropertyValue("Name").toString());

	xmlStreamWriter->writeStartElement("Time");
	QString unitsStr = unitList_.value(propertyContainer_->getPropertyValue("Units").toInt(),"unknown");
	xmlStreamWriter->writeAttribute("units",unitsStr);
	xmlStreamWriter->writeCharacters(QString("%1").arg(propertyContainer_->getPropertyValue("Time").toInt()));
	xmlStreamWriter->writeEndElement(); //Time

	xmlStreamWriter->writeEndElement(); //ControlElement

	return true;
}

//! Turns an XML fragment into a StopwatchController
bool StopwatchController::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "ControlElement")
	{
		addError("StopwatchController","Incorrect tag, expected <ControlElement>",xmlStreamReader);
		return false;
	}
	if(xmlStreamReader->attributes().value("type").toString() != ControllerType())
	{
		addError("StopwatchController","Incorrect type of controller",xmlStreamReader);
		return false;
	}

	if(xmlStreamReader->attributes().value("operatorVisible").toString() == "true")
		operatorVisible_ = true;
	else
		operatorVisible_ = false;

	if(xmlStreamReader->attributes().value("subjectVisible").toString() == "true")
		subjectVisible_ = true;
	else
		subjectVisible_ = false;

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "ControlElement") && !xmlStreamReader->atEnd())
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
			setName(xmlStreamReader->readElementText());
		}
		else if(name == "Time")
		{
			int unitIndex;
			unitIndex = unitList_.indexOf(xmlStreamReader->attributes().value("units").toString());
			if(unitIndex < 0)
			{
				addError("StopwatchController","Time contained unrecognized units",xmlStreamReader);
				return false;
			}
			propertyContainer_->setPropertyValue("Units",unitIndex);
			propertyContainer_->setPropertyValue("Time",xmlStreamReader->readElementText().toInt());
		}
		else
		{
			addError("StopwatchController", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("StopwatchController", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	return true;

}

} //namespace Picto