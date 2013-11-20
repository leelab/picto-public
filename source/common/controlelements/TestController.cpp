#include <QDebug>

#include "TestController.h"

namespace Picto
{
TestController::TestController()
{
	
	//AddDefinableProperty("Type",ControllerType());	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	AddDefinableProperty(QVariant::Int,"NumberOfFrames",120);

	//Make sure to update the list of results...
	addRequiredResult("Success");
}


ControlElement* TestController::NewTestController()
{
	return new TestController;
}

QSharedPointer<Asset> TestController::Create()
{
	return QSharedPointer<Asset>(new TestController());
}

QString TestController::ControllerType()
{
	return "Test Controller";
}

bool TestController::isDone(QSharedPointer<Engine::PictoEngine> engine)
{
	Q_UNUSED(engine);

	int numberOfFrames = propertyContainer_->getPropertyValue("NumberOfFrames").toInt();

	timesCalled_++;

	//qDebug()<<"Checking isDone for frame: "<<timesCalled_;

	if(timesCalled_ < numberOfFrames)
	{
		return false;
	}
	else
	{
		isDone_ = true;
		return true;
	}
}

QSharedPointer<Result> TestController::getResult()
{
	if(isDone_)
		return ResultContainer::getResult("Success");
	else
		return QSharedPointer<Result>();
}

void TestController::start(QSharedPointer<Engine::PictoEngine> engine)
{
	Q_UNUSED(engine);
	qDebug()<<"Starting Test Controller";
	timesCalled_ = 0;
	isDone_ = false;
}

void TestController::postDeserialize()
{
	ControlElement::postDeserialize();
}

bool TestController::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ControlElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

} //namespace Picto