#include "DirectorStatusManager.h"
#include "../../common/protocol/ProtocolCommand.h"
#include "../../common/network/CommandChannel.h"
#include "../../common/timing/timestamper.h"
#include "../../common/storage/AlignmentDataUnit.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

void DirectorStatusManager::setEngine(QSharedPointer<Picto::Engine::PictoEngine> engine)
{
	engine_ = engine;
	alignmentCode_ = 0;
	alignmentID_ = 0;
	lastAlignTime_ = QDateTime::currentDateTime();
}
QSharedPointer<Picto::Engine::PictoEngine> DirectorStatusManager::getEngine()
{
	Q_ASSERT(!engine_.isNull());
	return engine_.toStrongRef();
}
void DirectorStatusManager::setExperiment(QSharedPointer<Picto::Experiment> experiment)
{
	experiment_ = experiment;
}
QSharedPointer<Picto::Experiment> DirectorStatusManager::getExperiment()
{
	return experiment_;
}

void DirectorStatusManager::setUserInfo(QString info)
{
	if(engine_.isNull())
		return;
	QList<QSharedPointer<Picto::RenderingTarget> > renderingTargets = getEngine()->getRenderingTargets();
	foreach(QSharedPointer<Picto::RenderingTarget> target, renderingTargets)
	{
		target->updateStatus(info);
		target->showSplash();
	}
}

void DirectorStatusManager::setStatus(ComponentStatus status)
{
	ComponentStatus oldStatus = getStatus();
	ComponentStatusManager::setStatus(status);
	ComponentStatus newStatus = getStatus();
	if(newStatus <= idle)
	{
		switch(newStatus)
		{
		case disconnected:
			setUserInfo(QString("No Connection to Server"));
			break;
		case idle:
			setUserInfo(QString("Connected to Server"));
			break;
		}
		
	}
}

void DirectorStatusManager::newSession()
{
	alignmentCode_ = 0;
	alignmentID_ = 0;
	lastAlignTime_ = QDateTime::currentDateTime();
}
void DirectorStatusManager::doServerUpdate()
{
	//We keep on setting status/user info here just in case the director got minimized.  These actions will cause the splash
	//screen to re-present which will cause the director to remaximize and take focus.  I really shouldn't be talking about the
	//director here in its parent, but this will be useful 
	if(getStatus() <= idle)
		setStatus(getStatus());
	else if(getStatus() == stopped)
		setUserInfo("Engine Stopped");

	QSharedPointer<CommandChannel> dataChannel = getEngine()->getDataCommandChannel();
	if(dataChannel.isNull())
		return;
	if((getStatus() > stopped) && (lastAlignTime_.secsTo(QDateTime::currentDateTime()) > 5))
	{
		alignmentCode_ = (alignmentCode_ == 0x7F)? 0 : alignmentCode_+1;
		alignmentID_++;
		Timestamper timestamper;
		getEngine()->generateEvent(alignmentCode_);
		double timestamp = timestamper.stampSec();

		//Create an alignment command
		QSharedPointer<ProtocolCommand> command(new ProtocolCommand());
		command->setMethod("PUTDATA");
		command->setProtocolName("PICTO");
		command->setProtocolVersion("1.0");

		//Create the content of the alignment command
		QByteArray alignDataXml;
		QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&alignDataXml));
		
		AlignmentDataUnit alignData;
		alignData.setAlignCode(alignmentCode_);
		alignData.setAlignNumber(alignmentID_);
		alignData.setTimestamp(timestamp);

		xmlWriter->writeStartElement("Data");
		alignData.toXml(xmlWriter);
		xmlWriter->writeEndElement();

		command->setContent(alignDataXml);
		command->setFieldValue("Content-Length",QString::number(alignDataXml.length()));

		dataChannel->sendRegisteredCommand(command);
		lastAlignTime_ = QDateTime::currentDateTime();
	}
	else
	{
		QSharedPointer<Picto::ProtocolCommand> updateCommand(new Picto::ProtocolCommand());
		updateCommand->setMethod("COMPONENTUPDATE");
		updateCommand->setProtocolName("PICTO");
		updateCommand->setProtocolVersion("1.0");
		dataChannel->sendCommand(updateCommand);
	}
}