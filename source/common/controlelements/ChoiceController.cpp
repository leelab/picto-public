#include "ChoiceController.h"
#include "../engine/PictoEngine.h"
#include "../storage/PropertyFactory.h"

using namespace Picto;

ChoiceController::ChoiceController()
{
	//Properties
	//propertyContainer_->setPropertyValue("Type",ControllerType());
	AddDefinableProperty("Name","");
	AddDefinableProperty(QVariant::String,"SignalChannel","");
	AddDefinableProperty(QVariant::String,"Shape","");

	shapeList_ << "Rectangle" << "Ellipse";
	AddDefinableProperty(QtVariantPropertyManager::enumTypeId(),"Shape",0,"enumNames",shapeList_);

	unitList_ << "Sec" << "Ms" << "Us";
	AddDefinableProperty(QtVariantPropertyManager::enumTypeId(),"TimeUnits",0,"enumNames",unitList_);

	AddDefinableProperty(QVariant::Int,"FixationTime",1);
	timeUnits_ = 0;

	AddDefinableProperty(QVariant::Int,"TotalTime",5);

	reentriesAllowedList_ << "No" << "Yes";
	AddDefinableProperty(QtVariantPropertyManager::enumTypeId(),"AllowReentries",0,"enumNames",reentriesAllowedList_);

	AddDefinableProperty(QVariant::Bool,"OperatorVisible",false);
	AddDefinableProperty(QVariant::Bool,"SubjectVisible",false);
	AddDefinableProperty(QVariant::Rect,"Target",QRect(),0,-1);

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
	int targetNum = propertyContainer_->getPropertyList().filter("Target").length();
	targetNum++;

	//confirm that the new target name doesn't conflict
	if(results_.contains(targetName))
		return false;
	if(targetName == "NotATarget")
		return false;

	//Add the new properties
	QString namePropertyStr = QString("TargetName%1").arg(targetNum);
	QString targetPropertyStr = QString("Target%1").arg(targetNum);

	propertyContainer_->addProperty(QVariant::String,namePropertyStr, targetName);
	propertyContainer_->addProperty(QVariant::Rect,targetPropertyStr, target);

	results_.append(targetName);

	return true;
}

void ChoiceController::start(QSharedPointer<Engine::PictoEngine> engine)
{
	cumulativeTimer_.start();
	isDone_ = false;
	result_ = "";
	targetAcquired_ = false;

	//We call isDone to initialize everything
	isDone(engine);

}

bool ChoiceController::isDone(QSharedPointer<Engine::PictoEngine> engine)
{
	Controller::TimerUnits::TimerUnits timeUnits;
	if(unitList_.value(timeUnits_,"") == "Sec")
		timeUnits = Controller::TimerUnits::sec;
	else if(unitList_.value(timeUnits_,"") == "Ms")
		timeUnits = Controller::TimerUnits::ms;
	else if(unitList_.value(timeUnits_,"") == "Us")
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

	QString currTarget = insideTarget(engine);

	//just left a target
	if(currTarget != lastTarget_ && targetAcquired_)
	{
		//Are reentries allowed?
		if("No" == reentriesAllowedList_.value(propertyContainer_->getPropertyValue("AllowReentries").toInt(),"No"))
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
		int fixTime = propertyContainer_->getPropertyValue("FixationTime").toInt();
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
QString ChoiceController::insideTarget(QSharedPointer<Engine::PictoEngine> engine)
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

	//Run through the targets checking them
	foreach(QSharedPointer<Property> target, targets_)
	{
		QRect targetRect = target->value().toRect();
		if(checkSingleTarget(targetRect))
		{
			QString targetName = target->name();
			return targetName;
		}
	}
	//int numTargets = propertyContainer_->getPropertyList().filter("TargetName").length();
	//for(int target=1; target<= numTargets; target++)
	//{
	//	QRect targetRect = propertyContainer_->getPropertyValue(QString("Target%1").arg(target)).toRect();
	//	if(checkSingleTarget(targetRect))
	//	{
	//		QString targetName = propertyContainer_->getPropertyValue(QString("TargetName%1").arg(target)).toString();
	//		return targetName;
	//	}
	//}

	return "NotATarget";

}

//! \Brief checks a single target to determine if the subject's focus is inside it.
bool ChoiceController::checkSingleTarget(QRect targetRect)
{
	int x = signal_->peekValue("xpos");
	int y = signal_->peekValue("ypos");

	if("Rectangle" == shapeList_.value(shapeIndex_))
	{
		if(targetRect.contains(x,y))
			return true;
		else
			return false;
	}
	else if("Ellipse" == shapeList_.value(shapeIndex_))
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

///*! \Brief turns a choice controller into XML
// *
// *	The XML for a choice controller will look like this:
// * 	<ControlElement type="ChoiceController" operatorVisible="true subjectVisible="false">
// *		<Name>MyControlElement</Name>
// *		<SignalChannel>ChannelName</SignalChannel>
// *		<Shape>Rectangle</Shape>
// *		<TotalTime units="Ms">10000</TotalTime>
// *		<FixationTime units="Ms">1000</FixationTime>
// *		<AllowReentries>Yes</AllowReentries>
// *		<Target1 name="FirstTarget" x="400" y="800" width="100" height="100">
// *		<Target2 name="SecondTarget" x="800" y="800" width="100" height="100">
// *	</ControlElement>
//
// */

bool ChoiceController::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QString name = propertyContainer_->getPropertyValue("Name").toString();
	setName(name);
	shapeIndex_ = propertyContainer_->getPropertyValue("Shape").toInt();
	if(shapeIndex_ <0)
	{
		addError("ChoiceController","Unrecognized shape",xmlStreamReader);
		return false;
	}

	timeUnits_ = propertyContainer_->getPropertyValue("TimeUnits").toInt();
	if(timeUnits_<0)
	{
		addError("ChoiceController", "Unrecognized units for " + name.toAscii(),xmlStreamReader);
		return false;
	}

	QString targetName;
	QList<QSharedPointer<Serializable>> targetChildren = getGeneratedChildren("Target");
	foreach(QSharedPointer<Serializable> target, targetChildren)
	{
		targetName = target.staticCast<Property>()->name();
		if(targetName == "NotATarget")
		{
			addError("ChoiceController", "Target name cannot be 'NotATarget'", xmlStreamReader);
			return false;
		}
		//confirm that the new target name doesn't conflict
		if(results_.contains(targetName))
		{
			addError("ChoiceController", "No two Targets can have the same name.", xmlStreamReader);
			return false;
		}
		results_.append(targetName);
		targets_.push_back(target.staticCast<Property>());
	}

	return true;
}