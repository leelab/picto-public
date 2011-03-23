#include "DirectorStatusManager.h"
#include "../../common/protocol/ProtocolCommand.h"
#include "../../common/network/CommandChannel.h"
#include "../../common/timing/timestamper.h"
#include "../../common/storage/alignmentdatastore.h"
using namespace Picto;

void DirectorStatusManager::setEngine(QSharedPointer<Picto::Engine::PictoEngine> engine)
{
	engine_ = engine;
	alignmentCode_ = 0;
	alignmentID_ = 0;
	lastAlignTime_ = QTime::currentTime();
}
QSharedPointer<Picto::Engine::PictoEngine> DirectorStatusManager::getEngine()
{
	return engine_;
}
void DirectorStatusManager::setExperiment(QSharedPointer<Picto::Experiment> experiment)
{
	experiment_ = experiment;
}
QSharedPointer<Picto::Experiment> DirectorStatusManager::getExperiment()
{
	return experiment_;
}
void DirectorStatusManager::updateSplashStatus(QString status)
{
	if(engine_.isNull())
		return;
	QList<QSharedPointer<Picto::RenderingTarget> > renderingTargets = engine_->getRenderingTargets();
	foreach(QSharedPointer<Picto::RenderingTarget> target, renderingTargets)
	{
		target->updateStatus(status);
		target->showSplash();
	}
}
void DirectorStatusManager::newSession()
{
	alignmentCode_ = 0;
	alignmentID_ = 0;
	lastAlignTime_ = QTime::currentTime();
}
void DirectorStatusManager::doServerUpdate()
{
	QSharedPointer<CommandChannel> dataChannel = engine_->getDataCommandChannel();
	if(dataChannel.isNull())
		return;
	if((getStatus() > stopped) && (lastAlignTime_.secsTo(QTime::currentTime()) > 5))
	{
		alignmentCode_ = (alignmentCode_ == 0x7F)? 0 : alignmentCode_+1;
		alignmentID_++;
		Timestamper timestamper;
		engine_->generateEvent(alignmentCode_);
		double timestamp = timestamper.stampSec();

		//Create an alignment command
		QSharedPointer<ProtocolCommand> command(new ProtocolCommand());
		command->setMethod("PUTDATA");
		command->setProtocolName("PICTO");
		command->setProtocolVersion("1.0");

		//Create the content of the alignment command
		QByteArray alignDataXml;
		QSharedPointer<QXmlStreamWriter> xmlWriter(new QXmlStreamWriter(&alignDataXml));
		
		AlignmentDataStore alignData;
		alignData.setAlignCode(alignmentCode_);
		alignData.setAlignNumber(alignmentID_);
		alignData.setTimestamp(timestamp);

		xmlWriter->writeStartElement("Data");
		alignData.serializeAsXml(xmlWriter);
		xmlWriter->writeEndElement();

		command->setContent(alignDataXml);
		command->setFieldValue("Content-Length",QString::number(alignDataXml.length()));

		dataChannel->sendRegisteredCommand(command);
		lastAlignTime_ = QTime::currentTime();
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