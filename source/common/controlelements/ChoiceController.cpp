#include "ChoiceController.h"
#include "../engine/PictoEngine.h"
#include "../storage/PropertyFactory.h"
#include "../common/storage/ObsoleteAsset.h"

using namespace Picto;

ChoiceController::ChoiceController()
{
	//Properties
	//propertyContainer_->setPropertyValue("Type",ControllerType());
	
	AddDefinableProperty(QVariant::String,"SignalChannel","");
	//AddDefinableProperty(QVariant::String,"Shape","");

	//shapeList_ << "Rectangle" << "Ellipse";
	//AddDefinableProperty(PropertyContainer::enumTypeId(),"Shape",0,"enumNames",shapeList_);

	unitList_ << "Sec" << "Ms" << "Us";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"TimeUnits",0,"enumNames",unitList_);

	AddDefinableProperty(QVariant::Int,"FixationTime",1);
	timeUnits_ = 0;

	AddDefinableProperty(QVariant::Int,"TotalTime",5);

	AddDefinableProperty(QVariant::Bool,"AllowReentries",false);
	AddDefinableProperty(QVariant::Bool,"OnTarget",false);
	AddDefinableProperty(QVariant::Bool,"OnTargetChanged",false);

	//Target Entry and Exit Scripts are obsolete as of design syntax version "0.0.3".
	//This functionality should be handled in Frame Scripts by calling the 
	//newly exposed userOnTarget, userEnteredTarget, userExitedTarget functions.
	//AddDefinableProperty(QVariant::String,"TargetEntryScript","");
	//AddDefinableProperty(QVariant::String,"TargetExitScript","");
	AddDefinableObjectFactory("TargetEntryScript",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
	AddDefinableObjectFactory("TargetExitScript",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));

	defineResultFactoryType("Target",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ControlResult::Create))));

	//resultFactory_->addAssetType("Target",
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ControlResult::Create))));
	//setMaxUntypedResults(2);
	//Make sure to update the list of results...
	addRequiredResult("Broke Fixation");
	addRequiredResult("Total Time Exceeded");
	setMaxOptionalResults(0,"");
}


ControlElement* ChoiceController::NewChoiceController()
{
	return new ChoiceController;
}

QSharedPointer<Asset> ChoiceController::Create()
{
	return QSharedPointer<Asset>(new ChoiceController());
}

QString ChoiceController::ControllerType()
{
	return "Choice Controller";
}

void ChoiceController::start(QSharedPointer<Engine::PictoEngine> engine)
{
	cumulativeTimer_.start();
	isDone_ = false;
	result_ = "";
	targetAcquired_ = false;
	activateTargets();
	frameCtr_ = 0;

	//We call isDonePrivate to initialize everything
	isDonePrivate(engine);

}
void ChoiceController::stop(QSharedPointer<Engine::PictoEngine> engine)
{
	deactivateTargets();
}

void ChoiceController::activateTargets()
{
	foreach(QSharedPointer<ControlResult> target, targets_)
		target->setActive(true);
}

void ChoiceController::deactivateTargets()
{
	foreach(QSharedPointer<ControlResult> target, targets_)
		target->setActive(false);
}

void ChoiceController::upgradeVersion(QString deserializedVersion)
{
	ControlElement::upgradeVersion(deserializedVersion);
	if(deserializedVersion < "0.0.3")
	{	// In design syntax version "0.0.3", we added userOnTarget, userEnteredTarget, userExitedTarget functions.
		// In an effort to keep the allowed script locations uniform, we are making TargetEntry
		// and TargetExit scripts obsolete.
		// We upgrade these older experiments by automatically adding their entry and exit scripts
		// to the frame script and exit scripts in if(userEnteredTarget()) and if(userExitedTarget) blocks.

		//WARNING!!! userOnTarget(), userEnteredTarget() and userExitedTarget() 
		//functions cannot be used in AnalysisScripts of experiments that were saved with version below 0.0.3.  
		//This is because experiments with those versions used TargetEntry and TargetExit scripts and did not 
		//save OnTarget or OffTarget data in their session files.  Attempting to use these functions in Analysis 
		//Scripts would cause return of invalid data.
		QSharedPointer<Property> parentFrameScriptProp = getParentAsset().staticCast<DataStore>()->getGeneratedChildren("FrameScript").first().staticCast<Property>();
		QSharedPointer<Property> parentExitScriptProp = getParentAsset().staticCast<DataStore>()->getGeneratedChildren("ExitScript").first().staticCast<Property>();
		Q_ASSERT(parentFrameScriptProp);
		QString parentFrameScript = parentFrameScriptProp->value().toString();
		QString parentExitScript = parentExitScriptProp->value().toString();
		QSharedPointer<ObsoleteAsset> obsScript;
		if(getGeneratedChildren("TargetExitScript").size())
			obsScript = getGeneratedChildren("TargetExitScript").first().staticCast<ObsoleteAsset>();
		if(obsScript && obsScript->getValue().size())
		{
			//Put exit script into parent's frame script.
			parentFrameScript.prepend(QString("if(%1.userExitedTarget()){\n%2\n}\n").arg(getName()).arg(obsScript->getValue()));
			//Put exit script into parent's exit script.
			parentExitScript.prepend(QString("if(%1.userExitedTarget()){\n%2\n}\n").arg(getName()).arg(obsScript->getValue()));
		}
		obsScript.clear();
		if(getGeneratedChildren("TargetEntryScript").size())
			obsScript = getGeneratedChildren("TargetEntryScript").first().staticCast<ObsoleteAsset>();
		if(obsScript && obsScript->getValue().size())
		{
			//Put entry script into parent's frame script.
			parentFrameScript.prepend(QString("if(%1.userEnteredTarget()){\n%2\n}\n").arg(getName()).arg(obsScript->getValue()));
			//Put entry script into parent's exit script.
			parentExitScript.prepend(QString("if(%1.userEnteredTarget()){\n%2\n}\n").arg(getName()).arg(obsScript->getValue()));
		}
		parentFrameScriptProp->setValue(parentFrameScript);
		parentExitScriptProp->setValue(parentExitScript);
	}
}

bool ChoiceController::isDone(QSharedPointer<Engine::PictoEngine> engine)
{
	//This property tells us if the user entered or exited the target this frame.
	//It may have been set during start(), so don't change it on the first frame
	//since no one has had a chance to read it yet.  Afterwards, set it back to false
	//every frame.
	if(frameCtr_ > 0)
		propertyContainer_->getProperty("OnTargetChanged")->setValue(false);
	frameCtr_++;
	return isDonePrivate(engine);
}

bool ChoiceController::isDonePrivate(QSharedPointer<Engine::PictoEngine> engine)
{
	//This property tells us if the user entered or exited the target this frame.  Initialize it to false.
	propertyContainer_->getProperty("OnTargetChanged")->setValue(false);

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
	int fixTime = propertyContainer_->getPropertyValue("FixationTime").toInt();
	int currAcqTime = acquisitionTimer_.elapsedTime(timeUnits);
	int remainingFixTime = targetAcquired_?fixTime-currAcqTime:fixTime;
	if(currTotalTime+remainingFixTime > totalTime)
	{
		isDone_ = true;
		result_ = "Total Time Exceeded";
		return true;
	}

	QString currTarget = insideTarget(engine);

	//just left a target
	if(currTarget != lastTarget_ && targetAcquired_)
	{
		if(!engine->slaveMode())
		{
			propertyContainer_->getProperty("OnTarget")->setValue(false);
			propertyContainer_->getProperty("OnTargetChanged")->setValue(true);
			//if(propertyContainer_->getPropertyValue("TargetExitScript").toString() != "")
			//	runScript(getName().simplified().remove(' ').append("_TargetExit"));
		}
		//Are reentries allowed?
		if(!propertyContainer_->getPropertyValue("AllowReentries").toBool())
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
		
		//Recheck if fixation is possible in the timeframe since a target was left
		if(currTotalTime+fixTime > totalTime)
		{
			isDone_ = true;
			result_ = "Total Time Exceeded";
			return true;
		}
	}
	//just entered a target
	else if(currTarget != "NotATarget" && !targetAcquired_)
	{
		if(!engine->slaveMode())
		{
			propertyContainer_->getProperty("OnTarget")->setValue(true);
			propertyContainer_->getProperty("OnTargetChanged")->setValue(true);
			//if(propertyContainer_->getPropertyValue("TargetEntryScript").toString() != "")
			//	runScript(getName().simplified().remove(' ').append("_TargetEntry"));
		}
		targetAcquired_ = true;
		acquisitionTimer_.start();
		//If fixation time is zero, we're done
		if(fixTime <= 0)
		{
			isDone_ = true;
			result_ = currTarget;
			return true;
		}
	}
	//staying inside a target
	else if(currTarget == lastTarget_ && targetAcquired_)
	{
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
	Q_ASSERT(!engine.isNull());
	if(signal_.isNull())
	{
		//grab the signal channel
		signal_ = engine->getSignalChannel(propertyContainer_->getPropertyValue("SignalChannel").toString());
		Q_ASSERT_X(!signal_.isNull(),"ChoiceController::insideTarget","There is no signal channel called: " + propertyContainer_->getPropertyValue("SignalChannel").toString().toLatin1());

		//confirm that the signal channel contains "x" and "y" subchannels
		Q_ASSERT(signal_->getSubchannels().contains("x"));
		Q_ASSERT(signal_->getSubchannels().contains("y"));
	}

	//Run through the targets checking them
	foreach(QSharedPointer<ControlResult> target, targets_)
	{
		//QRect targetRect = target->getBounds();
		if(target->contains(signal_->peekValue("x"),signal_->peekValue("y"))/*checkSingleTarget(targetRect)*/)
		{
			QString targetName = target->getName();
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


////! \Brief checks a single target to determine if the subject's focus is inside it.
//bool ChoiceController::checkSingleTarget(QRect targetRect)
//{
//	int x = signal_->peekValue("x");
//	int y = signal_->peekValue("y");
//
//	if("Rectangle" == shapeList_.value(shapeIndex_))
//	{
//		if(targetRect.contains(x,y))
//			return true;
//		else
//			return false;
//	}
//	else if("Ellipse" == shapeList_.value(shapeIndex_))
//	{
//		/*We're going to test this equation:
//			(x-x0)^2     (y-0)^2
//			--------  +  --------  <= 1
//			 width^2     height^2
//		*/
//		double x0 = targetRect.x() + targetRect.width()/2;
//		double y0 = targetRect.y() + targetRect.height()/2;
//		double width = targetRect.width();
//		double height = targetRect.height();
//		double term1 = (x-x0)*(x-x0)/(width*width);
//		double term2 = (y-y0)*(y-y0)/(height*height);
//		if(term1 + term2 <= 1.0)
//			return true;
//		else
//			return false;
//	}
//	else
//	{
//		return false;
//	}
//
//}

QString ChoiceController::getResult()
{
	return result_;
}

bool ChoiceController::userOnTarget()
{
	return propertyContainer_->getPropertyValue("OnTarget").toBool();
}

bool ChoiceController::userEnteredTarget()
{
	return userOnTarget() && propertyContainer_->getPropertyValue("OnTargetChanged").toBool();
}

bool ChoiceController::userExitedTarget()
{
	return !userOnTarget() && propertyContainer_->getPropertyValue("OnTargetChanged").toBool();
}

void ChoiceController::postDeserialize()
{
	ControlElement::postDeserialize();
	
	//Don't let user see OnTarget/OnTargetChanged, they are for internal use only
	propertyContainer_->getProperty("OnTarget")->setVisible(false);
	propertyContainer_->getProperty("OnTargetChanged")->setVisible(false);

	//shapeIndex_ = propertyContainer_->getPropertyValue("Shape").toInt();
	timeUnits_ = propertyContainer_->getPropertyValue("TimeUnits").toInt();

	QString targetName;
	QList<QSharedPointer<Asset>> targetChildren = getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> target, targetChildren)
	{
		if(target->assetType() == "ControlResult")
		{
			targets_.push_back(target.staticCast<ControlResult>());
			//addChildScriptableContainer(target.staticCast<ScriptableContainer>());
		}
	}
	setPropertyRuntimeEditable("FixationTime");
	setPropertyRuntimeEditable("TotalTime");
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
	if(!ControlElement::validateObject(xmlStreamReader))
		return false;
	
	bool hasValidChild = false;
	QList<QSharedPointer<Asset>> targetChildren = getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> target, targetChildren)
	{
		if(target->assetType() == "ControlResult")
		{
			hasValidChild = true;
		}
	}
	if(!hasValidChild)
	{
		addError("ChoiceController", "At least one Control Result must be defined inside this ChoiceController.");
		return false;
	}
	if(timeUnits_<0)
	{
		addError("ChoiceController", "Unrecognized units for " + getName().toLatin1(),xmlStreamReader);
		return false;
	}

	QString targetName;
	QMap<QString,bool> targetNameMap;
	foreach(QSharedPointer<Asset> target, targetChildren)
	{
		if(target->assetType() != "ControlResult")
		{
			if((target->getName() != "Broke Fixation")
				&& (target->getName() != "Total Time Exceeded"))
			{
				addError("ChoiceController", "Only Target Results can be used in a ChoiceController.", xmlStreamReader);
				return false;
			}
		}
		targetName = target->getName();
		if(targetName == "NotATarget")
		{
			addError("ChoiceController", "Target name cannot be 'NotATarget'", xmlStreamReader);
			return false;
		}
		//confirm that the new target name doesn't conflict
		if(targetNameMap.contains(targetName))
		{
			addError("ChoiceController", "No two Targets can have the same name.", xmlStreamReader);
			return false;
		}
		targetNameMap[targetName] = true;
	}
	return true;
}