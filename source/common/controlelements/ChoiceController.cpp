#include "ChoiceController.h"
#include "../engine/PictoEngine.h"

namespace Picto {

ChoiceController::ChoiceController()
{
	//Properties
	propertyContainer_.setPropertyValue("Type",ControllerType());

	propertyContainer_.addProperty(Property(QVariant::String,"SignalChannel",""));

	Property shapesEnumProperty(QtVariantPropertyManager::enumTypeId(),"Shape",0);
	shapeList_ << "Rectangle" << "Ellipse";
	shapesEnumProperty.addAttribute("enumNames",shapeList_);
	propertyContainer_.addProperty(shapesEnumProperty);

	Property unitsEnumProperty(QtVariantPropertyManager::enumTypeId(),"TimeUnits",0);
    unitList_ << "Sec" << "Ms" << "Us";
	unitsEnumProperty.addAttribute("enumNames", unitList_);
	propertyContainer_.addProperty(unitsEnumProperty);


	propertyContainer_.addProperty(Property(QVariant::Int,"FixationTime",1));
	propertyContainer_.addProperty(Property(QVariant::Int,"TotalTime",5));

	Property reentriesAllowedEnumProperty(QtVariantPropertyManager::enumTypeId(),"AllowReentries",0);
    reentriesAllowedList_ << "No" << "Yes";
	reentriesAllowedEnumProperty.addAttribute("enumNames", reentriesAllowedList_);
	propertyContainer_.addProperty(reentriesAllowedEnumProperty);


	//Make sure to update the list of results...
	results_.append("Broke Fixation");
	results_.append("Total Time Excceeded");
}


ControlElement* ChoiceController::NewChoiceController()
{
	return new ChoiceController;
}

QString ChoiceController::ControllerType()
{
	return "Choice Controller";
}

bool ChoiceController::addTarget(QString targetName, QRect target)
{
	//Figure out what target number we're on...
	int targetNum = propertyContainer_.getPropertyList().filter("TargetName").length();
	targetNum++;

	//confirm that the new target name doesn't conflict
	if(results_.contains(targetName))
		return false;
	if(targetName == "NotATarget")
		return false;

	//Add the new properties
	QString namePropertyStr = QString("TargetName%1").arg(targetNum);
	QString targetPropertyStr = QString("Target%1").arg(targetNum);

	propertyContainer_.addProperty(Property(QVariant::String,namePropertyStr, targetName));
	propertyContainer_.addProperty(Property(QVariant::Rect,targetPropertyStr, target));

	results_.append(targetName);
}


void ChoiceController::start()
{
	cumulativeTimer_.start();
	isDone_ = false;
	result_ = "";
	targetAcquired_ = false;

	//We call isDone to initialize everything
	isDone();

}

bool ChoiceController::isDone()
{
	Controller::TimerUnits::TimerUnits timeUnits;
	if(unitList_.value(propertyContainer_.getPropertyValue("TimeUnits").toInt(),"") == "Sec")
		timeUnits = Controller::TimerUnits::sec;
	else if(unitList_.value(propertyContainer_.getPropertyValue("TimeUnits").toInt(),"") == "Ms")
		timeUnits = Controller::TimerUnits::ms;
	else if(unitList_.value(propertyContainer_.getPropertyValue("TimeUnits").toInt(),"") == "Us")
		timeUnits = Controller::TimerUnits::us;
	else
		Q_ASSERT(false);


	//check to see if we've met or exceeded the total time
	int totalTime = propertyContainer_.getPropertyValue("TotalTime").toInt();
	int currTotalTime = cumulativeTimer_.elapsedTime(timeUnits);
	if(currTotalTime >= totalTime)
	{
		isDone_ = true;
		result_ = "Total Time Excceeded";
		return true;
	}

	QString currTarget = insideTarget();

	//just left a target
	if(currTarget != lastTarget_ && targetAcquired_)
	{
		//Are reentries allowed?
		if("No" == reentriesAllowedList_.value(propertyContainer_.getPropertyValue("AllowReentries").toInt(),"No"))
		{
			isDone_ = true;
			result_ = "Broke Fixation";
			return true;
		}
		if(currTarget == "NotATarget")
			targetAcquired_ = false;
		else
		{
			acquisitionTimer_.start();
			targetAcquired_ = true;
		}
	}
	//just entered a target
	else if(currTarget != "NotATarget" && !targetAcquired_)
	{
		targetAcquired_ = true;
		acquisitionTimer_.start();
	}
	//staying inside a target
	else if(currTarget == lastTarget_ && targetAcquired_)
	{
		int fixTime = propertyContainer_.getPropertyValue("FixationTime").toInt();
		int currAcqTime = acquisitionTimer_.elapsedTime(timeUnits);

		if(currAcqTime > fixTime)
		{
			isDone_ = true;
			result_ = currTarget;
			return true;
		}
	}
	//staying outside a target
	else if(currTarget == "NotATarget" && !targetAcquired_)
	{
		//do nothing
	}
	lastTarget_ = currTarget;

	return false;
}

/*! \Brief Returns the target name that we are currently inside.  
 *
 *	If we aren't inside a target, this returns NotATarget(which is a reserved name).
 */
QString ChoiceController::insideTarget()
{
	if(signal_.isNull())
	{
		//grab the signal channel
		signal_ = Engine::PictoEngine::getSignalChannel(propertyContainer_.getPropertyValue("SignalChannel").toString());
		if(signal_.isNull())
			return false;

		//confirm that the signal channel contains "xpos" and "ypos" subchannels
		Q_ASSERT(signal_->getSubchannels().contains("xpos"));
		Q_ASSERT(signal_->getSubchannels().contains("ypos"));
		signal_->start();
	}

	//Run through the targets checking them
	int numTargets = propertyContainer_.getPropertyList().filter("TargetName").length();
	for(int target=1; target<= numTargets; target++)
	{
		QRect targetRect = propertyContainer_.getPropertyValue(QString("Target%1").arg(target)).toRect();
		if(checkSingleTarget(targetRect))
		{
			QString targetName = propertyContainer_.getPropertyValue(QString("TargetName%1").arg(target)).toString();
			return targetName;
		}
	}
	return "NotATarget";

}

//! \Brief checks a single target to determine if the subject's focus is inside it.
bool ChoiceController::checkSingleTarget(QRect targetRect)
{
	int x = signal_->peekValue("xpos");
	int y = signal_->peekValue("ypos");

	if("Rectangle" == shapeList_.value(propertyContainer_.getPropertyValue("Shape").toInt()))
	{
		if(targetRect.contains(x,y))
			return true;
		else
			return false;
	}
	else if("Ellipse" == shapeList_.value(propertyContainer_.getPropertyValue("Shape").toInt()))
	{
		/*We're going to test this equation:
			(x-x0)^2     (y-0)^2
			--------  +  --------  <= 1
			 width^2     height^2
		*/
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

QString ChoiceController::getResult()
{
	return result_;
}

/*! \Brief turns a choice controller into XML
 *
 *	The XML for a choice controller will look like this:
 * 	<ControlElement type="ChoiceController" operatorVisible="true subjectVisible="false">
 *		<Name>MyControlElement</Name>
 *		<SignalChannel>ChannelName</SignalChannel>
 *		<Shape>Rectangle</Shape>
 *		<TotalTime units="Ms">10000</TotalTime>
 *		<FixationTime units="Ms">1000</FixationTime>
 *		<AllowReentries>Yes</AllowReentries>
 *		<Target1 name="FirstTarget" x="400" y="800" width="100" height="100">
 *		<Target2 name="SecondTarget" x="800" y="800" width="100" height="100">
 *	</ControlElement>

 */
bool ChoiceController::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("ControlElement");
	xmlStreamWriter->writeAttribute("type",propertyContainer_.getPropertyValue("Type").toString());
	if(operatorVisible_)
		xmlStreamWriter->writeAttribute("operatorVisible","true");
	else
		xmlStreamWriter->writeAttribute("operatorVisible","false");
	if(subjectVisible_)
		xmlStreamWriter->writeAttribute("subjectVisible","true");
	else
		xmlStreamWriter->writeAttribute("subjectVisible","false");

	xmlStreamWriter->writeTextElement("Name", propertyContainer_.getPropertyValue("Name").toString());
	xmlStreamWriter->writeTextElement("SignalChannel", propertyContainer_.getPropertyValue("SignalChannel").toString());

	QString shapeStr = unitList_.value(propertyContainer_.getPropertyValue("Shape").toInt(),"unknown");
	xmlStreamWriter->writeTextElement("Shape", shapeStr);

	xmlStreamWriter->writeStartElement("TotalTime");
	QString timeUnitsStr = unitList_.value(propertyContainer_.getPropertyValue("TimeUnits").toInt(), "");
	xmlStreamWriter->writeAttribute("units",timeUnitsStr);
	xmlStreamWriter->writeCharacters(propertyContainer_.getPropertyValue("TotalTime").toString());
	xmlStreamWriter->writeEndElement();
	
	xmlStreamWriter->writeStartElement("FixationTime");
	xmlStreamWriter->writeAttribute("units",timeUnitsStr);
	xmlStreamWriter->writeCharacters(propertyContainer_.getPropertyValue("FixationTime").toString());
	xmlStreamWriter->writeEndElement();

	QString reacqAllowedStr = unitList_.value(propertyContainer_.getPropertyValue("AllowReentries").toInt(),"unknown");
	xmlStreamWriter->writeTextElement("AllowReentries", shapeStr);

	int numTargets = propertyContainer_.getPropertyList().filter("TargetName").length();
	for(int target=1; target <= numTargets; target++)
	{
		QRect targetRect = propertyContainer_.getPropertyValue(QString("Target%1").arg(target)).toRect();
		QString targetName = propertyContainer_.getPropertyValue(QString("TargetName%1").arg(target)).toString();

		xmlStreamWriter->writeStartElement("Target");
		xmlStreamWriter->writeAttribute("name", targetName);
		xmlStreamWriter->writeAttribute("x",QString("%1").arg(targetRect.x()));
		xmlStreamWriter->writeAttribute("y",QString("%1").arg(targetRect.y()));
		xmlStreamWriter->writeAttribute("width",QString("%1").arg(targetRect.width()));
		xmlStreamWriter->writeAttribute("height",QString("%1").arg(targetRect.height()));
		xmlStreamWriter->writeEndElement();
	}
	
	return true;
}

bool ChoiceController::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
		//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "ControlElement")
	{
		addError("ChoiceController","Incorrect tag, expected <ControlElement>",xmlStreamReader);
		return false;
	}
	if(xmlStreamReader->attributes().value("type").toString() != ControllerType())
	{
		addError("ChoiceController","Incorrect type of controller",xmlStreamReader);
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
			propertyContainer_.setPropertyValue("SignalChannel",xmlStreamReader->readElementText());
		}
		else if(name == "Shape")
		{
			int shapeIndex= shapeList_.indexOf(xmlStreamReader->readElementText());
			if(shapeIndex <0)
			{
				addError("ChoiceController","Unrecognized shape",xmlStreamReader);
				return false;
			}
			propertyContainer_.setPropertyValue("Shape",shapeIndex);
		}
		else if(name == "TotalTime" ||
				name == "FixationTime")
		{
			QString unitsStr = xmlStreamReader->attributes().value("units").toString();
			int unitsIndex = unitList_.indexOf(unitsStr);

			if(unitsIndex<0)
			{
				addError("ChoiceController", "Unrecognized units for " + name,xmlStreamReader);
				return false;
			}

			if(units == "")
			{
				units = unitList_.value(unitsIndex);
				propertyContainer_.setPropertyValue("TimeUnits",unitsIndex);
			}
			else if(units != unitList_.value(unitsIndex))
			{
				addError("ChoiceController", "Only one unit of time is allowed per controller",xmlStreamReader);
				return false;
			}

			propertyContainer_.setPropertyValue(name,xmlStreamReader->readElementText().toInt());
		}
		else if(name == "AllowReentries")
		{
			int reentriesAllowedListIndex = reentriesAllowedList_.indexOf(xmlStreamReader->readElementText());
			if(reentriesAllowedListIndex <0)
			{
				addError("ChoiceController","Unrecognized value in AllowReentries",xmlStreamReader);
				return false;
			}
			propertyContainer_.setPropertyValue("AllowReentries",reentriesAllowedListIndex);
		}
		else if(name.startsWith("Target"))
		{
			QString targetName = xmlStreamReader->attributes().value("name").toString();

			QRect target;
			target.setX(xmlStreamReader->attributes().value("x").toString().toInt());
			target.setY(xmlStreamReader->attributes().value("y").toString().toInt());
			target.setWidth(xmlStreamReader->attributes().value("width").toString().toInt());
			target.setHeight(xmlStreamReader->attributes().value("height").toString().toInt());

			addTarget(targetName,target);
		}
		else
		{
			addError("ChoiceController", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();

	}

	if(xmlStreamReader->atEnd())
	{
		addError("ChoiceController", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	return true;

	return false;
}

}; //namespace Picto
