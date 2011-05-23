#include <QDebug>

#include "TestController.h"

namespace Picto
{
TestController::TestController()
{
	AddDefinableProperty("Name","");
	AddDefinableProperty("Type",ControllerType());	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	AddDefinableProperty(QVariant::Bool,"OperatorVisible",false);
	AddDefinableProperty(QVariant::Bool,"SubjectVisible",false);
	AddDefinableProperty(QVariant::Int,"NumberOfFrames",120);

	//Make sure to update the list of results...
	results_.append("Success");
}


ControlElement* TestController::NewTestController()
{
	return new TestController;
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

QString TestController::getResult()
{
	if(isDone_)
		return "Success";
	else
		return "";
}

void TestController::start(QSharedPointer<Engine::PictoEngine> engine)
{
	Q_UNUSED(engine);
	qDebug()<<"Starting Test Controller";
	timesCalled_ = 0;
	isDone_ = false;
}


/*! \brief Turns the ControlElement into an XML fragment
 *
 * A serialized TestController will look something like this:
 *	<ControlElement type="TestController" operatorVisible="true subjectVisible="false">
 *		<Name>MyControlElement</Name>
 *		<NumberOfFrames>120</NumberOfFrames>
 *	</ControlElement>
 */
//
//bool TestController::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("ControlElement");
//	xmlStreamWriter->writeAttribute("type",propertyContainer_->getPropertyValue("Type").toString());
//	if(operatorVisible_)
//		xmlStreamWriter->writeAttribute("operatorVisible","true");
//	else
//		xmlStreamWriter->writeAttribute("operatorVisible","false");
//	if(subjectVisible_)
//		xmlStreamWriter->writeAttribute("subjectVisible","true");
//	else
//		xmlStreamWriter->writeAttribute("subjectVisible","false");
//
//	xmlStreamWriter->writeTextElement("Name", propertyContainer_->getPropertyValue("Name").toString());
//	xmlStreamWriter->writeTextElement("NumberOfFrames",propertyContainer_->getPropertyValue("NumberOfFrames").toString());
//
//	xmlStreamWriter->writeEndElement(); //ControlElement
//
//	return true;
//}
//
//bool TestController::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "ControlElement")
//	{
//		addError("TestController","Incorrect tag, expected <ControlElement>",xmlStreamReader);
//		return false;
//	}
//	if(xmlStreamReader->attributes().value("type").toString() != ControllerType())
//	{
//		addError("TestController","Incorrect type of controller",xmlStreamReader);
//		return false;
//	}
//
//	if(xmlStreamReader->attributes().value("operatorVisible").toString() == "true")
//		operatorVisible_ = true;
//	else
//		operatorVisible_ = false;
//
//	if(xmlStreamReader->attributes().value("subjectVisible").toString() == "true")
//		subjectVisible_ = true;
//	else
//		subjectVisible_ = false;
//
//	xmlStreamReader->readNext();
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "ControlElement") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//
//		QString name = xmlStreamReader->name().toString();
//		if(name == "Name")
//		{
//			setName(xmlStreamReader->readElementText());
//		}
//		else if(name == "NumberOfFrames")
//		{
//			propertyContainer_->setPropertyValue("NumberOfFrames",xmlStreamReader->readElementText().toInt());
//		}
//		else
//		{
//			addError("TestController", "Unexpected tag", xmlStreamReader);
//			return false;
//		}
//		xmlStreamReader->readNext();
//	}
//
//	if(xmlStreamReader->atEnd())
//	{
//		addError("TestController", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//
//	return true;
//
//}

bool TestController::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	operatorVisible_ = propertyContainer_->getPropertyValue("OperatorVisible").toBool();
	subjectVisible_ = propertyContainer_->getPropertyValue("SubjectVisible").toBool();
	return true;
}

} //namespace Picto