#include "State.h"
#include "../controlelements/ControlElementFactory.h"

#include <QDebug>

namespace Picto {

State::State()
: scene_(QSharedPointer<Scene>(new Scene))
{
	propertyContainer_.setPropertyValue("Type","State");
	propertyContainer_.addProperty(Property(QVariant::String,"EntryScript",""));
	propertyContainer_.addProperty(Property(QVariant::String,"FrameScript",""));
	propertyContainer_.addProperty(Property(QVariant::String,"ExitScript",""));

	//at some point, these should actually do something, but for the moment
	//we'll leave them as -1 to show that they aren't being used
	revision_ = -1;
	engineNeeded_ = -1;

}

QString State::run()
{
	//Figure out which scripts we will be running
	bool runEntryScript = !propertyContainer_.getPropertyValue("EntryScript").toString().isEmpty();
	bool runFrameScript = !propertyContainer_.getPropertyValue("FrameScript").toString().isEmpty();
	bool runExitScript = !propertyContainer_.getPropertyValue("ExitScript").toString().isEmpty();

	QString entryScriptName = getName().simplified().remove(' ')+"Entry";
	QString frameScriptName = getName().simplified().remove(' ')+"Frame";
	QString exitScriptName = getName().simplified().remove(' ')+"Exit";

	//run the entry script
	if(runEntryScript)
	{
		qsEngine_->globalObject().property(entryScriptName).call();
		if(qsEngine_->hasUncaughtException())
		{
			QString errorMsg = "Uncaught exception in State" + getName() +" entry script.\n";
			errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
											  .arg(qsEngine_->uncaughtException().toString());
			errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
			qDebug()<<errorMsg;
		}
	}

	//Start up all of the control elements
	foreach(QSharedPointer<ControlElement> control, controlElements_)
	{
		control->start();
	}

	QString result = "";
	bool isDone = false;
	while(!isDone)
	{
		//Draw the scene
		scene_->render();

		//check all of the control elements
		foreach(QSharedPointer<ControlElement> control, controlElements_)
		{
			if(control->isDone())
			{
				result = control->getResult();
				isDone = true;
			}
		}
		//run the frame script
		if(runFrameScript && !isDone)
		{
			qsEngine_->globalObject().property(frameScriptName).call();
			if(qsEngine_->hasUncaughtException())
			{
				QString errorMsg = "Uncaught exception in State" + getName() +" frame script.\n";
				errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
												  .arg(qsEngine_->uncaughtException().toString());
				errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
				qDebug()<<errorMsg;
			}
		}

		//If there are no control elements, then we just return with an empty string
		if(controlElements_.isEmpty())
			isDone = true;
	}

	//run the exit script
	if(runExitScript)
	{
		qsEngine_->globalObject().property(exitScriptName).call();
		if(qsEngine_->hasUncaughtException())
		{
			QString errorMsg = "Uncaught exception in State" + getName() +" exit script.\n";
			errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
											  .arg(qsEngine_->uncaughtException().toString());
			errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
			qDebug()<<errorMsg;
		}
	}

	return result;
}

/* \Brief Sets up the state for scripting
 *
 *	This gets called before we actually start running a state machine.
 *	Because of scoping issues, each State gets its own scriptengine
 *	To initialize a state, we need to do the following:
 *		1. Bind the parameters to the engine
 *		2. Bind the contained visual elements to the engine
 *		3. Create functions in the engine for the 3 (potential) scripts.
 *	To minimize wasted resources, the state doesn't use a concrete QScriptEngine
 *	but insteads instantiates one if the state needs to run scripts.  The
 *	assumption is that many states (at least 50%) won't need the scripting
 *	functionality.
 */
bool State::initScripting(QScriptEngine &qsEngine)
{
	//Since we have our own engine, we aren't using the passed in one
	Q_UNUSED(qsEngine);

	QStringList scriptTypes;
	scriptTypes<<"Entry"<<"Frame"<<"Exit";

	//Figure out which scripts we will be running
	//(The odd array structure is useful later...)
	bool scriptsToRun[3];
	scriptsToRun[0] = !propertyContainer_.getPropertyValue("EntryScript").toString().isEmpty();
	scriptsToRun[1] = !propertyContainer_.getPropertyValue("FrameScript").toString().isEmpty();
	scriptsToRun[2] = !propertyContainer_.getPropertyValue("ExitScript").toString().isEmpty();

	//If we aren't running any scripts, we're done
	bool noScripts = true;
	for(int i=0; i<sizeof(scriptsToRun); i++)
	{
		if(scriptsToRun[i])
		{
			noScripts = false;
			break;
		}
	}
	if(noScripts)
		return true;


	//create the engine
	qsEngine_ = QSharedPointer<QScriptEngine>(new QScriptEngine());

	//Bind our parameters
	parameterContainer_.bindToScriptEngine(*qsEngine_);

	//Bind the visualElements
	scene_->bindToScriptEngine(qsEngine_);

	//create functions for our scripts
	for(int i=0; i<3; i++)
	{
		if(!scriptsToRun[i])
			continue;

		//We allow ScriptElement names to have whitespace, so we need 
		//to get rid of it for a script function name
		QString functionName = getName().simplified().remove(' ')+scriptTypes[i];

		//confirm that this function doesn't already exist in the engine
		//If you hit this assert, it means that there are two ScriptElements
		//with the asme name.  The GUI shouldn't allow this...
		if(qsEngine_->globalObject().property(functionName).isValid())
		{
			return false;
		}

		QString script = propertyContainer_.getPropertyValue(scriptTypes[i]+"Script").toString();
		QString function = "function " + functionName + "() { " + script + "}";

		//add the function to the engine by calling evaluate on it
		qsEngine_->evaluate(function);
		if(qsEngine_->hasUncaughtException())
		{
			QString errorMsg = "Uncaught exception in State" + getName() + scriptTypes[i] + " script.\n";
			errorMsg += QString("Line %1: %2\n").arg(qsEngine_->uncaughtExceptionLineNumber())
											  .arg(qsEngine_->uncaughtException().toString());
			errorMsg += QString("Backtrace: %1\n").arg(qsEngine_->uncaughtExceptionBacktrace().join(", "));
			qDebug()<<errorMsg;
		}
	}

	return true;
}

void State::addControlElement(QSharedPointer<ControlElement> controlElement)
{
	//Add the control element to our map
	controlElements_.insert(controlElement->getName(),controlElement);

	//Add the controlElement's results to our list of possible results
	QStringList controlElementResults = controlElement->getResultList();
	results_ = results_ +controlElementResults;
}

void State::removeControlElement(QString controlElementName)
{
	if(!controlElements_.contains(controlElementName))
		return;

	//remove the results affiliated with this control element
	foreach(QString result, controlElements_[controlElementName]->getResultList())
	{
		results_.removeOne(result);
	}

	//remove the element
	controlElements_.remove(controlElementName);


}



/*!	\Brief Turns a state object into an XML framgent.
 *
 *	The XML fragment for a state will look like this:
 * 
 *	<StateMachineElement type="State">
 *		<Revision>1</Revision>
 *		<EngineNeeded>1</EngineNeeded>
 *		<Name>Fixation</Name>
 *		<ControlElements>
 *			<ControlElement type="TargetController">
 *				...
 *			</ControlElement>
 *		</ControlElements>
 *		<Scene>
 *			...
 *		</Scene>
 *		<Scripts>
 *			<Script type="entry"> ... </Script>
 *			<Script type="frame "> ... </Script>
 *			<Script type="exit"> ... </Script>
 *		</Scripts>

 *		<Results>
 *			<!-- We'll list all possible results from the ControlElements here -->
 *			<Result>
 *				<Name>..</Name>
 *			</Result>
 *		</Results>
 *	</StateMachineElement>
 */
bool State::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("StateMachineElement");
	xmlStreamWriter->writeAttribute("type","State");
	
	xmlStreamWriter->writeTextElement("Revision",QString("%1").arg(revision_));
	xmlStreamWriter->writeTextElement("EngineNeeded",QString("%1").arg(engineNeeded_));
	xmlStreamWriter->writeTextElement("Name", propertyContainer_.getPropertyValue("Name").toString());

	xmlStreamWriter->writeStartElement("ControlElements");
	foreach(QSharedPointer<ControlElement> c, controlElements_)
	{
		c->serializeAsXml(xmlStreamWriter);
	}
	xmlStreamWriter->writeEndElement();

	scene_->serializeAsXml(xmlStreamWriter);

	serializeResults(xmlStreamWriter);

	xmlStreamWriter->writeEndElement(); //State
	return true;
}

/*!	\Brief Turns a an XML fragment into a State object */
bool State::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "StateMachineElement")
	{
		addError("FlowElement","Incorrect tag, expected <StateMachineElement>",xmlStreamReader);
		return false;
	}
	if(xmlStreamReader->attributes().value("type").toString() != type())
	{
		addError("FlowElement","Incorrect type of StateMachineElement, expected State",xmlStreamReader);
		return false;
	}

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "StateMachineElement") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}

		QString name = xmlStreamReader->name().toString();
		if(name == "Revision")
		{
			bool ok;
			revision_= xmlStreamReader->readElementText().toInt(&ok);
			if(!ok)
			{
				addError("State","Revision value not an integer", xmlStreamReader);
				return false;
			}
		}
		else if(name == "EngineNeeded")
		{
			bool ok;
			engineNeeded_= xmlStreamReader->readElementText().toInt(&ok);
			if(!ok)
			{
				addError("State","EngineNeeded value not an integer", xmlStreamReader);
				return false;
			}
		}
		else if(name == "Name")
		{
			propertyContainer_.setPropertyValue("Name",QVariant(xmlStreamReader->readElementText()));
		}
		else if(name == "ControlElements")
		{
			//do nothing
		}
		else if(name == "ControlElement")
		{
			ControlElementFactory factory;
			QString controllerType = xmlStreamReader->attributes().value("type").toString();
			QSharedPointer<ControlElement> newController = factory.generateControlElement(controllerType);
			if(!newController)
			{
				addError("State", "Failed to create a new Controller of type "+controllerType,xmlStreamReader);
				return false;
			}
			if(!newController->deserializeFromXml(xmlStreamReader))
			{
				addError("State", "Failed to deserialize <ControlElement>", xmlStreamReader);
				return false;
			}
			addControlElement(newController);
		}
		else if(name == "Scene")
		{
			QSharedPointer<Scene> newScene(new Scene);
			if(!newScene->deserializeFromXml(xmlStreamReader))
			{
				addError("State", "Failed to deserialize <Scene>", xmlStreamReader);
				return false;
			}
			scene_ = newScene;
		}
		else if(name == "Scripts")
		{
			//do nothing
		}
		else if(name == "Script")
		{
			QString type = xmlStreamReader->attributes().value("type").toString();
			QString script = xmlStreamReader->readElementText();

			if(type == "entry")
			{
				propertyContainer_.setPropertyValue("EntryScript",script);
			}
			else if(type == "frame")
			{
				propertyContainer_.setPropertyValue("FrameScript",script);
			}
			else if(type == "exit")
			{
				propertyContainer_.setPropertyValue("ExitScript",script);
			}
			else
			{
				addError("State", "Unrecognized script type.  Expecting \"entry\", \"frame\", or \"exit\"", xmlStreamReader);
				return false;
			}
		}
		else if(name == "Results")
		{
			if(!deserializeResults(xmlStreamReader))
			{
				addError("State", "Failed to deserialize <Results>", xmlStreamReader);
				return false;
			}
		}

		else
		{
			addError("State", "Unexpected tag", xmlStreamReader);
			return false;
		}
		xmlStreamReader->readNext();
	}

	if(xmlStreamReader->atEnd())
	{
		addError("State", "Unexpected end of document", xmlStreamReader);
		return false;
	}

	return true;
}


}; //namespace Picto
