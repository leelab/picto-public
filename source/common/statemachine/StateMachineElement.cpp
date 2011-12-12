#include <QCoreApplication>
#include "StateMachineElement.h"

#include "../property/Property.h"
#include "../protocol/ProtocolCommand.h"
#include "../protocol/ProtocolResponse.h"
#include "../storage/StateDataUnit.h"
#include "../storage/AssetFactory.h"
#include "../memleakdetect.h"

namespace Picto {

	QString StateMachineElement::lastTransitionTime_;

StateMachineElement::StateMachineElement()
{
	//DefinePlaceholderTag("Results");
	//AddDefinableObjectFactory("Result",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(Result::Create))) );
}

QPoint StateMachineElement::getDisplayLayoutPosition()
{
	return layoutPosition_;
}

QString StateMachineElement::type()
{
	QString typeStr = propertyContainer_->getPropertyValue("Type").toString();
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
//QString StateMachineElement::getMasterStateResult(QSharedPointer<Engine::PictoEngine> engine)
//{
//	Q_ASSERT(false);
//	return "";
//	//Collect the data from the server
//	//Note that below we use 6 places after the time decimal point.  We do this because we get
//	//6 places after the decimal point and we need to make sure that new data will be after old data.
//	//The reason we need this level of precision is that we found that some transitions were occuring at
//	//the same millisecond, and we need these to be differentiated.
//	QString commandStr = QString("GETDATA StateDataUnit:%1 PICTO/1.0").arg(lastTransitionTime_);
//	QSharedPointer<Picto::ProtocolCommand> command(new Picto::ProtocolCommand(commandStr));
//	QSharedPointer<Picto::ProtocolResponse> response;
//
//	CommandChannel* slaveToServerChan = engine->getSlaveCommandChannel();
//
//	slaveToServerChan->sendRegisteredCommand(command);
//	QString commandID = command->getFieldValue("Command-ID");
//	//Get the response to this command
//	do
//	{
//		if(!slaveToServerChan->waitForResponse(50))
//		{
//			return "";
//		}
//		response = slaveToServerChan->getResponse();
//	}while(!response || response->getFieldValue("Command-ID") != commandID);
//
//	//Response not 200:OK
//	if(response->getResponseCode() != Picto::ProtocolResponseType::OK)
//		return "";
//	
//	QByteArray xmlFragment = response->getContent();
//	QSharedPointer<QXmlStreamReader> xmlReader(new QXmlStreamReader(xmlFragment));
//
//	while(!xmlReader->atEnd() && xmlReader->readNext() && xmlReader->name() != "Data");
//
//	if(xmlReader->atEnd())
//		return "";
//
//
//	xmlReader->readNext();
//	while(!xmlReader->isEndElement() && xmlReader->name() != "Data" && !xmlReader->atEnd())
//	{
//		if(xmlReader->name() == "StateDataUnit")
//		{
//			StateDataUnit data;
//			data.fromXml(xmlReader);
//			Q_ASSERT(data.getTime() != lastTransitionTime_);
//
//			//Even if multiple transitions have stacked up, we are expecting that data to be
//			//in temporal order, so we can simply return the first response.  However, we are
//			//going to confirm that the source name matches as well
//			QString msg = QString("Transition source: %1 doesn't match current state: %2 at time: %3")
//				.arg(data.getSource()).arg(getName()).arg(data.getTime());
//			Q_ASSERT_X(data.getSource() == getName() || data.getSource().toUpper() == "NULL","StateMachineElement::getMasterStateResult", 
//				msg.toAscii());
//			lastTransitionTime_ = data.getTime();
//
//			QString result = data.getDestination();
//			if(result == "NULL")
//				result = "";
//			if(result != "EngineAbort")
//				result.prepend(data.getMachinePath()+"::");
//
//			//The result will be "NULL" if this was the starting "transition"
//			if(result.toUpper() == "NULL")
//				return "";
//			else
//			{
//				QString msg = QString("Path: %1, Source: %2, SourceResult: %3, Destination: %4").arg(data.getMachinePath()).arg(data.getSource()).arg(data.getSourceResult()).arg(data.getDestination());
//				if(data.getSource() != getName() || data.getSource().toUpper() == "NULL")
//					msg = QString("Transition source: %1 doesn't match current state: %2 at time: %3")
//						.arg(data.getSource()).arg(getName()).arg(data.getTime());
//				qDebug(msg.toAscii());
//				return result;
//			}
//		}
//		else
//		{
//			return "";
//		}
//	}
//
//	return "";
//
//}

void StateMachineElement::postDeserialize()
{
	ResultContainer::postDeserialize();
}

bool StateMachineElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ResultContainer::validateObject(xmlStreamReader))
		return false;
	QString resultName;
	QStringList results = getResultList();
	foreach(QString result,results)
	{
		if(result == "EngineAbort")
		{
			addError("StateMachineElement", "EngineAbort is a resticted keyword, and may not be used as the name of a result", xmlStreamReader);
			return false;
		}
	}
	return true;
}

}; //namespace Picto
