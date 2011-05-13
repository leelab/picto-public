#include "TargetController.h"
#include "../engine/PictoEngine.h"

namespace Picto
{
TargetController::TargetController()
{
	//Properties
	propertyContainer_->setPropertyValue("Type",ControllerType());

	propertyContainer_->addProperty(QVariant::String,"SignalChannel","");

	QSharedPointer<Property> shapesEnumProperty = propertyContainer_->addProperty(QtVariantPropertyManager::enumTypeId(),"Shape",0);
	shapeList_ << "Rectangle" << "Oval";
	shapesEnumProperty->addAttribute("enumNames",shapeList_);
	

	propertyContainer_->addProperty(QVariant::Rect, "Target",QRect(0,0,100,100));

	QSharedPointer<Property> unitsEnumProperty = propertyContainer_->addProperty(QtVariantPropertyManager::enumTypeId(),"TimeUnits",0);
    unitList_ << "Sec" << "Ms" << "Us";
	unitsEnumProperty->addAttribute("enumNames", unitList_);


	propertyContainer_->addProperty(QVariant::Int,"FixationTime",1);
	propertyContainer_->addProperty(QVariant::Int,"TotalTime",5);
	propertyContainer_->addProperty(QVariant::Int,"MinInitia AcquisitionTime",0);
	propertyContainer_->addProperty(QVariant::Int,"MaxInitialAcquisitionTime",1);
	propertyContainer_->addProperty(QVariant::Int,"MaxReacquisitionTime",0);

	QSharedPointer<Property> reaqcAllowedEnumProperty = propertyContainer_->addProperty(QtVariantPropertyManager::enumTypeId(),"ReacquisitionAllowed",0);
    reacquisitionAllowedList_ << "No" << "Yes";
	reaqcAllowedEnumProperty->addAttribute("enumNames", reacquisitionAllowedList_);


	//Make sure to update the list of results...
	results_.append("Success");
	results_.append("Broke Fixation");
	results_.append("Total Time Excceeded");
	results_.append("Initial Aquistion Time Exceeded");
	results_.append("Reaquistion Time Exceeded");
}


ControlElement* TargetController::NewTargetController()
{
	return new TargetController;
}

QString TargetController::ControllerType()
{
	return "Target Controller";
}


void TargetController::start(QSharedPointer<Engine::PictoEngine> engine)
{
	cumulativeTimer_.start();
	isDone_ = false;
	result_ = "";
	targetAcquired_ = false;
	waitingForReacquisition_ = false;
	initialAcquisitionOccurred_ = false;

	//We call isDone to initialize everything
	isDone(engine);
}

bool TargetController::isDone(QSharedPointer<Engine::PictoEngine> engine)
{
	Controller::TimerUnits::TimerUnits timeUnits;
	if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Sec")
		timeUnits = Controller::TimerUnits::sec;
	else if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Ms")
		timeUnits = Controller::TimerUnits::ms;
	else if(unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(),"") == "Us")
		timeUnits = Controller::TimerUnits::us;
	else
		Q_ASSERT(false);


	//check to see if we've met or exceeded the total time
	int totalTime = propertyContainer_->getPropertyValue("TotalTime").toInt();
	int currTotalTime = cumulativeTimer_.elapsedTime(timeUnits);
	if(currTotalTime >= totalTime)
	{
		isDone_ = true;
		result_ = "Total Time Excceeded";
		return true;
	}

	bool isInsideTarget = insideTarget(engine);

	//just entered target
	if(!targetAcquired_ && isInsideTarget)
	{
		//if we're past the Min Initial Acquisition Time mark the target as acquired
		//and start running the timer
		int minAcqTime = propertyContainer_->getPropertyValue("MinInitialAcquisitionTime").toInt();
		int currTime = cumulativeTimer_.elapsedTime(timeUnits);
		if(currTime >= minAcqTime)
		{
			acquisitionTimer_.start();
			targetAcquired_ = true;
			initialAcquisitionOccurred_ = true;

		}

	}
	//just left target
	else if(targetAcquired_ && !isInsideTarget)
	{
		targetAcquired_ = false;

		//If reacquisition isn't allowed, then we're done with a failure value.
		if("No" == reacquisitionAllowedList_.value(propertyContainer_->getPropertyValue("ReacquisitionAllowed").toInt(),"No"))
		{
			isDone_ = true;
			result_ = "Broke Fixation";
			return true;
		}
		else
		{
			waitingForReacquisition_ = true;
			reacquisitionTimer_.start();
		}
	}
	//staying inside target
	else if(targetAcquired_ && isInsideTarget)
	{
		//check to see if we've met or exceeded the fixation time
		int fixTime = propertyContainer_->getPropertyValue("FixationTime").toInt();
		int currAcqTime = acquisitionTimer_.elapsedTime(timeUnits);
		if(currAcqTime >= fixTime)
		{
			isDone_ = true;
			result_ = "Success";
			return true;
		}

	}
	//staying outside target
	else if(!targetAcquired_ && !isInsideTarget)
	{
		//check to see if we've met or exceeded the total time
		int maxAcqTime = propertyContainer_->getPropertyValue("MaxInitialAcquisitionTime").toInt();
		int currTotalTime = cumulativeTimer_.elapsedTime(timeUnits);

		//check to see if we've met or exceeded the max acquisition time (if relevant)
		if(!initialAcquisitionOccurred_)
		{
			if(currTotalTime >= maxAcqTime)
			{
				isDone_ = true;
				result_ = "Initial Aquistion Time Exceeded";
				return true;
			}
		}

		//check to see if we've met or exceeded the reacquisition time
		if(waitingForReacquisition_)
		{
			int reAcqTime = propertyContainer_->getPropertyValue("MaxReacquisitionTime").toInt();
			int currReAcqTime = reacquisitionTimer_.elapsedTime(timeUnits);
			if(currReAcqTime >= reAcqTime)
			{
				isDone_ = true;
				result_ = "Reaquistion Time Exceeded";
				return true;
			}
		}

	}

	return false;
}

QString TargetController::getResult()
{
	return result_;
}

/*!	\brief Returns true if the signalChannel coordinates are within the target area
 */
bool TargetController::insideTarget(QSharedPointer<Engine::PictoEngine> engine)
{
	if(engine.isNull())
		return false;
	if(signal_.isNull())
	{
		//grab the signal channel
		signal_ = engine->getSignalChannel(propertyContainer_->getPropertyValue("SignalChannel").toString());
		if(signal_.isNull())
			return false;

		//confirm that the signal channel contains "xpos" and "ypos" subchannels
		Q_ASSERT(signal_->getSubchannels().contains("xpos"));
		Q_ASSERT(signal_->getSubchannels().contains("ypos"));
	}

	int x = signal_->peekValue("xpos");
	int y = signal_->peekValue("ypos");

	if("Rectangle" == shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt()))
	{
		QRect targetRect = propertyContainer_->getPropertyValue("Target").toRect();
		if(targetRect.contains(x,y))
			return true;
		else
			return false;
	}
	else if("Ellipse" == shapeList_.value(propertyContainer_->getPropertyValue("Shape").toInt()))
	{
		/*We're going to test this equation:
			(x-x0)^2     (y-0)^2
			--------  +  --------  <= 1
			 width^2     height^2
		*/
		QRect targetRect = propertyContainer_->getPropertyValue("Target").toRect();
		double x0 = targetRect.x() + targetRect.width()/2;
		double y0 = targetRect.y() + targetRect.height()/2;
		double width = targetRect.width();
		double height = targetRect.height();
		double term1 = (x-x0)*(x-x0)/(width*width);
		double term2 = (y-y0)*(y-y0)/(height*height);
		if(term1 + term2 <= 1.0)
			return true;
		else
			return false;
	}
	else
	{
		return false;
	}
}


/*! \brief Turns the ControlElement into an XML fragment
 *
 * A serialized StopwatchController will look something like this:
 *	<ControlElement type="TargetController" operatorVisible="true subjectVisible="false">
 *		<Name>MyControlElement</Name>
 *		<SignalChannel>ChannelName</SignalChannel>
 *		<Shape>Rectangle</Shape>
 *		<Target x="400" y="800" width="100" height="100">
 *		<TotalTime units="Ms">10000</TotalTime>
 *		<FixationTime units="Ms">1000</FixationTime>
 *		<MinInitialAcquisitionTime units="Ms">200</MinInitialAcquisitionTime>
 *		<MaxInitialAcquisitionTime units="Ms">2000</MaxInitialAcquisitionTime>
 *		<MaxReacquisitionTime units="Ms">500</MaxReacquisitionTime>
 *		<ReacquisitionAllowed>Yes</ReacquisitionAllowed>
 *	</ControlElement>
 */
//
bool TargetController::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
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
	xmlStreamWriter->writeTextElement("SignalChannel", propertyContainer_->getPropertyValue("SignalChannel").toString());

	QString shapeStr = unitList_.value(propertyContainer_->getPropertyValue("Shape").toInt(),"unknown");
	xmlStreamWriter->writeTextElement("Shape", shapeStr);

	xmlStreamWriter->writeStartElement("Target");
	QRect target = propertyContainer_->getPropertyValue("Target").toRect();
	xmlStreamWriter->writeAttribute("x",QString("%1").arg(target.x()));
	xmlStreamWriter->writeAttribute("y",QString("%1").arg(target.y()));
	xmlStreamWriter->writeAttribute("width",QString("%1").arg(target.width()));
	xmlStreamWriter->writeAttribute("height",QString("%1").arg(target.height()));
	xmlStreamWriter->writeEndElement();

	xmlStreamWriter->writeStartElement("TotalTime");
	QString timeUnitsStr = unitList_.value(propertyContainer_->getPropertyValue("TimeUnits").toInt(), "");
	xmlStreamWriter->writeAttribute("units",timeUnitsStr);
	xmlStreamWriter->writeCharacters(propertyContainer_->getPropertyValue("TotalTime").toString());
	xmlStreamWriter->writeEndElement();
	
	xmlStreamWriter->writeStartElement("FixationTime");
	xmlStreamWriter->writeAttribute("units",timeUnitsStr);
	xmlStreamWriter->writeCharacters(propertyContainer_->getPropertyValue("FixationTime").toString());
	xmlStreamWriter->writeEndElement();
	
	xmlStreamWriter->writeStartElement("MinInitialAcquisitionTime");
	xmlStreamWriter->writeAttribute("units",timeUnitsStr);
	xmlStreamWriter->writeCharacters(propertyContainer_->getPropertyValue("MinInitialAcquisitionTime").toString());
	xmlStreamWriter->writeEndElement();
	
	xmlStreamWriter->writeStartElement("MaxInitialAcquisitionTime");
	xmlStreamWriter->writeAttribute("units",timeUnitsStr);
	xmlStreamWriter->writeCharacters(propertyContainer_->getPropertyValue("MaxInitialAcquisitionTime").toString());
	xmlStreamWriter->writeEndElement();
	
	xmlStreamWriter->writeStartElement("MaxReacquisitionTime");
	xmlStreamWriter->writeAttribute("units",timeUnitsStr);
	xmlStreamWriter->writeCharacters(propertyContainer_->getPropertyValue("MaxReacquisitionTime").toString());
	xmlStreamWriter->writeEndElement();

	QString reacqAllowedStr = unitList_.value(propertyContainer_->getPropertyValue("ReacquisitonAllowed").toInt(),"unknown");
	xmlStreamWriter->writeTextElement("ReacquisitionAllowed", shapeStr);

	xmlStreamWriter->writeEndElement(); //ControlElement

	return true;
}

//! Turns an XML fragment into a StopwatchController
bool TargetController::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "ControlElement")
	{
		addError("TargetController","Incorrect tag, expected <ControlElement>",xmlStreamReader);
		return false;
	}
	if(xmlStreamReader->attributes().value("type").toString() != ControllerType())
	{
		addError("TargetController","Incorrect type of controller",xmlStreamReader);
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

	QString units = "";

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
		else if(name == "SignalChannel")
		{
			propertyContainer_->setPropertyValue("SignalChannel",xmlStreamReader->readElementText());
		}
		else if(name == "Shape")
		{
			int shapeIndex= shapeList_.indexOf(xmlStreamReader->readElementText());
			if(shapeIndex <0)
			{
				addError("TargetController","Unrecognized shape",xmlStreamReader);
				return false;
			}
			propertyContainer_->setPropertyValue("Shape",shapeIndex);
		}
		else if(name == "Target")
		{
			QRect target;
			target.setX(xmlStreamReader->attributes().value("x").toString().toInt());
			target.setY(xmlStreamReader->attributes().value("y").toString().toInt());
			target.setWidth(xmlStreamReader->attributes().value("width").toString().toInt());
			target.setHeight(xmlStreamReader->attributes().value("height").toString().toInt());
			propertyContainer_->setPropertyValue("Target",target);
		}
		else if(name == "TotalTime" ||
				name == "FixationTime" ||
				name == "MinInitialAcquisitionTime" ||
				name == "MaxInitialAcquisitionTime" ||
				name == "MaxReacquisitionTime")
		{
			QString unitsStr = xmlStreamReader->attributes().value("units").toString();
			int unitsIndex = unitList_.indexOf(unitsStr);

			if(unitsIndex<0)
			{
				addError("TargetController", "Unrecognized units for " + name,xmlStreamReader);
				return false;
			}

			if(units == "")
			{
				units = unitList_.value(unitsIndex);
				propertyContainer_->setPropertyValue("TimeUnits",unitsIndex);
			}
			else if(units != unitList_.value(unitsIndex))
			{
				addError("TargetController", "Units for " + name + " are different from ealier units",xmlStreamReader);
				return false;
			}

			propertyContainer_->setPropertyValue(name,xmlStreamReader->readElementText().toInt());
		}
		else if(name == "ReacquisitionAllowed")
		{
			int reacquisitionAllowedIndex = reacquisitionAllowedList_.indexOf(xmlStreamReader->readElementText());
			if(reacquisitionAllowedIndex <0)
			{
				addError("TargetController","Unrecognized value in RecquisitionAllowed",xmlStreamReader);
				return false;
			}
			propertyContainer_->setPropertyValue("ReacquisitionAllowed",reacquisitionAllowedIndex);

		}
		else
		{
			addError("TargetController", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("TargetController", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	return true;

}

} //namespace Picto