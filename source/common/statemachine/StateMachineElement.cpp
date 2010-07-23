#include "StateMachineElement.h"

#include "../property/Property.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"
#include "../storage/StateDataStore.h"
#include "Result.h"

namespace Picto {

	double StateMachineElement::lastTransitionTime_;

StateMachineElement::StateMachineElement()
:propertyContainer_("StateMachineElement")
{
	propertyContainer_.addProperty(Property(QVariant::String,"Name",""));
	propertyContainer_.addProperty(Property(QVariant::String,"Type",""));
	defaultResult_ = "";
}

StateMachineElement::StateMachineElement(QSharedPointer<ParameterContainer> parameters)
:propertyContainer_("StateMachineElement")
{
	propertyContainer_.addProperty(Property(QVariant::String,"Name",""));
	propertyContainer_.addProperty(Property(QVariant::String,"Type",""));
	defaultResult_ = "";

	addParameters(parameters);
}

QPoint StateMachineElement::getDisplayLayoutPosition()
{
	return layoutPosition_;
}

/*! \Brief Adds a Result to the element
 *
 *	Each StateMachineElement has a finite number of predefined results.  A
 *	result must be added before it can be returned.  The addition of results
 *	can take place at construction (for elements with fixed results), or at 
 *  run time (for elements with results that may be edited by the user, like
 *	a FlowElement).
 */
bool StateMachineElement::addResult(QSharedPointer<Result> result)
{
	if(results_.contains(result->getName()))
		return false;
	else
	{
		results_.push_back(result->getName());
		return true;
	}
}

//! \brief Returns a list of all possible results.
QStringList StateMachineElement::getResultList()
{
	return results_;
}

/*! \brief Copies the parameters from the passed in container
 *	
 *	This is used when adding an element to a state machine.  By passing
 *	in the state machines parameter container, we now have access to all 
 *	of the state machine's parameters.  Effectively this allows us to
 *	have parameters with global scope.
 */
void StateMachineElement::addParameters(QSharedPointer<ParameterContainer> parameters)
{
	QStringList newParams = parameters->getParameterList();

	foreach(QString param, newParams)
	{
		parameterContainer_.addParameter(parameters->getParameter(param));
	}
}

void StateMachineElement::addParameters(ParameterContainer &parameters)
{
	QStringList newParams = parameters.getParameterList();

	foreach(QString param, newParams)
	{
		parameterContainer_.addParameter(parameters.getParameter(param));
	}
}

/*! \brief Sets the name of this StateMachineElement
 *
 *	Names will need to be unique within a state machine.  However,
 *	this can't be checked at this level, so we'll just set the name.
 */
void StateMachineElement::setName(QString name)
{
	propertyContainer_.setPropertyValue("Name", QVariant(name));
}

QString StateMachineElement::getName()
{
	QString name = propertyContainer_.getPropertyValue("Name").toString();
	return name;
}

QString StateMachineElement::type()
{
	QString typeStr = propertyContainer_.getPropertyValue("Type").toString();
	return typeStr;
}

/*! Checks the server for a result from the master engine/state machine
 *
 *	If we're running in slave mode, we'll be making calls to the server to figure
 *	out what is going on in the master state machine.  This function will be used
 *	by all of the derived classes to make those calls.
 *
 *	If the master has already left this state, then the result string is returned.
 *	otherwise, an empty string is returned.
 */
QString StateMachineElement::getMasterStateResult(QSharedPointer<Engine::PictoEngine> engine)
{

	//Collect the data from the server
	QString commandStr = QString("GETDATA StateDataStore:%1 PICTO/1.0").arg(lastTransitionTime_);
	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(commandStr));
	QSharedPointer<Picto::ProtocolResponse> response;

	CommandChannel* slaveToServerChan = engine->getSlaveCommandChannel();

	slaveToServerChan->sendCommand(command);
	//No response
	if(!slaveToServerChan->waitForResponse(1000))
		return "";

	response = slaveToServerChan->getResponse();

	//Response not 200:OK
	if(response->getResponseCode() != Picto::ProtocolResponseType::OK)
		return "";
	
	QByteArray xmlFragment = response->getContent();
	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlFragment));

	while(!xmlReader->atEnd() && xmlReader->readNext() && xmlReader->name() != "Data");

	if(xmlReader->atEnd())
		return "";


	xmlReader->readNext();
	while(!xmlReader->isEndElement() && xmlReader->name() != "Data" && !xmlReader->atEnd())
	{
		if(xmlReader->name() == "StateDataStore")
		{

			StateDataStore data;
			data.deserializeFromXml(xmlReader);
			
			//Even if multiple transitions have stacked up, we are expecting that data to be
			//in temporal order, so we can simply return the first response.  However, we are
			//going to confirm that the source name matches as well
			QString msg = QString("Transition source: %1 doesn't match current state: %2 at time: %3")
				.arg(data.getSource()).arg(getName()).arg(data.getTime());
			Q_ASSERT_X(data.getSource() == getName(),"StateMachineElement::getMasterStateResult", 
				msg.toAscii());
			lastTransitionTime_ = data.getTime();

			return data.getSourceResult();
		}
		else
		{
			return "";
		}
	}

	return "";

}

void StateMachineElement::processStatusDirective(QSharedPointer<Engine::PictoEngine> engine, QSharedPointer<ProtocolResponse> dataResponse)
{

		QString statusDirective = dataResponse->getDecodedContent().toUpper();

		//We may want to break this out in a seperate function at some point...
		if(statusDirective.startsWith("OK"))
		{
			//do nothing
		}
		else if(statusDirective.startsWith("STOP"))
		{
			engine->stop();
		}
		else if(statusDirective.startsWith("PAUSE"))
		{
			engine->pause();
		}
		else if(statusDirective.startsWith("RESUME"))
		{
			engine->resume();
		}
		else if(statusDirective.startsWith("REWARD"))
		{
			int channel = statusDirective.split(" ").value(1).toInt();
			engine->giveReward(channel);	
		}
		else
		{
			Q_ASSERT_X(false, "State::updateServer", "Unrecognized directive received from server");
		}
}

/*!	\brief A simple function to turn the result list into XML
 *
 *	When serializing a StateMachineElement, we sometimes need to list all possible
 *	results.  This function takes care of serializing the test list of
 *	results.  Note that if you are using a result as an element within 
 *	a state machine, this function probably shouldn't be used, since 
 *	you can call serialize on the result objects directly.
 */
bool StateMachineElement::serializeResults(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Results");

	foreach(QString result, results_)
	{
		xmlStreamWriter->writeStartElement("Result");
		xmlStreamWriter->writeTextElement("Name",result);
		xmlStreamWriter->writeEndElement(); //Result
	}

	if(!defaultResult_.isEmpty())
	{
		xmlStreamWriter->writeStartElement("Result");
		xmlStreamWriter->writeAttribute("type","default");
		xmlStreamWriter->writeTextElement("Name",defaultResult_);
		xmlStreamWriter->writeEndElement(); //Result
	}


	xmlStreamWriter->writeEndElement(); //Results
	return true;
}

/*!	\Brief A simple function to generate the result list from XML
 *
 *	This function may not be needed in all StateMachineElements (for example, the
 *	FlowElement generates the result list from the passed in conditions, and
 *	ignores this section of the XML)
 */
bool StateMachineElement::deserializeResults(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement())
		return false;
	if(xmlStreamReader->name() != "Results")
		return false;

	xmlStreamReader->readNext();
	bool isDefault = false;
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Results") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Result")
		{
			if(xmlStreamReader->attributes().value("type").toString() == "default")
				isDefault = true;
		}
		else if(name == "Name")
		{
			QString resultName = xmlStreamReader->readElementText();
			if(resultName == "EngineAbort")
			{
				addError("StateMachineElement", "EngineAbort is a resticted keyword, and may not be used as the name of a result", xmlStreamReader);
				return false;
			}

			if(isDefault)
			{
				defaultResult_ = resultName;
				isDefault = false;
			}
			else
			{
				//add the result name to our list, but only if it isn't already there...
				if(!results_.contains(resultName))
					results_.append(resultName);
			}
		}
		else
		{
			return false;
		}
		xmlStreamReader->readNext();
	}
	if(xmlStreamReader->atEnd())
		return false;
	return true;
}



}; //namespace Picto
