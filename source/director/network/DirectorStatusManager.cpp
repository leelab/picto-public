#include "DirectorStatusManager.h"
#include "../../common/protocol/ProtocolCommand.h"
#include "../../common/network/CommandChannel.h"
#include "../../common/timing/timestamper.h"
#include "../../common/storage/AlignmentDataUnit.h"
#include "../../common/memleakdetect.h"
#include "../frontpanel/FPInterface.h"
#include "../../common/globals.h"
using namespace Picto;

/*! \brief The number of seconds that must pass since the last alignment event before the next
 *	alignment event is sent by objects of this class.
 */
#define SECS_PRE_ALIGN 5

QString DirectorStatusManager::getName()
{
	if(engine_.isNull())
		return "";
	return getEngine()->getName();
}

/*! \brief Sets the Picto::Engine::PictoEngine that is used on this Director to run Experiments.*/
void DirectorStatusManager::setEngine(QSharedPointer<Picto::Engine::PictoEngine> engine)
{
	engine_ = engine;
	alignmentCode_ = 0;
	alignmentID_ = 0;
	lastAlignTime_ = QDateTime::currentDateTime();
	connect(engine.data(),SIGNAL(pauseRequested()),this,SLOT(pauseRequested()));
}

/*! \brief Sets the DirectorData that is used to store default reward quantities on this DirectorStatusManager. 
 *	This is used in COMPONENTUPDATE commands sent to the Picto Server.
 */
void DirectorStatusManager::setDirectorData(QSharedPointer<DirectorData> directorData)
{
	directorData_ = directorData;
}

/*! \brief Returns the Picto::Engine::PictoEngine that is used on this Director to run Experiments.*/
QSharedPointer<Picto::Engine::PictoEngine> DirectorStatusManager::getEngine()
{
	Q_ASSERT(!engine_.isNull());
	return engine_.toStrongRef();
}

/*! \brief Sets the current experiment being run by this Director.*/
void DirectorStatusManager::setExperiment(QSharedPointer<Picto::Experiment> experiment)
{
	experiment_ = experiment;
}

/*! \brief Gets the current experiment being run by this Director.*/
QSharedPointer<Picto::Experiment> DirectorStatusManager::getExperiment()
{
	return experiment_;
}

/*! \brief Extends ComponentStatusManager::setUserInfo() to display the input user information
 *	on the bottom left corner of the screen.
 */
void DirectorStatusManager::setUserInfo(QString info)
{
	if(engine_.isNull())
		return;
	QList<QSharedPointer<Picto::RenderingTarget> > renderingTargets = getEngine()->getRenderingTargets();
	//Add system number to user info
	info.prepend(QString("System Num: %1 - ").arg(Picto::portNums->getSystemNumber()));
	foreach(QSharedPointer<Picto::RenderingTarget> target, renderingTargets)
	{
		target->updateStatus(info);
		target->showSplash();
	}
}

/*! \brief Extends ComponentStatusManager::setStatus() to send the current Status to attached
 *	ControlPanelInterface objects and describe the current server connection in setUserInfo().
 */
void DirectorStatusManager::setStatus(ComponentStatus status)
{
	ComponentStatusManager::setStatus(status);
	ComponentStatus newStatus = getStatus();

	//Set new status to all control panels
	QString statusStr = getStatusAsString();
	QList<QSharedPointer<ControlPanelInterface>> cPans = engine_.toStrongRef()->getControlPanels();
	foreach(QSharedPointer<ControlPanelInterface> cPan,cPans)
	{
		cPan->statusChanged(statusStr);
	}
	
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

/*! \brief Implements ComponentStatusManager::newSession() to reset alignment event data.
*/
void DirectorStatusManager::newSession()
{
	alignmentCode_ = 0;
	alignmentID_ = 0;
	lastAlignTime_ = QDateTime::currentDateTime();
}

/*! \brief Implements ComponentStatusManager::doFrequentUpdate() to handle all commands coming into
*	attached ControlPanelInterface object.
*/
void DirectorStatusManager::doFrequentUpdate()
{
	if(engine_.isNull())
		return;
	foreach(QSharedPointer<ControlPanelInterface> cp,getEngine()->getControlPanels())
	{
		cp->doIncomingCommands();
	}
}

/*! \brief Implements ComponentStatusManager::doServerUpdate() to send alignment events to an attached neural system.  Also sends
 *	records of recent Alignment events and COMPONENTUPDATE "keep alive" commands to the Picto Server.
 */
void DirectorStatusManager::doServerUpdate()
{
	//We keep on setting status/user info here just in case the director got minimized.  These actions will cause the splash
	//screen to re-present which will cause the director to remaximize and take focus.
	if(getStatus() <= idle)
		setStatus(getStatus());
	else if(getStatus() == stopped)
		setUserInfo("Engine Stopped");

	QSharedPointer<CommandChannel> dataChannel = getEngine()->getDataCommandChannel();
	if(dataChannel.isNull())
		return;
	if((getStatus() > stopped) && (lastAlignTime_.secsTo(QDateTime::currentDateTime()) > SECS_PRE_ALIGN))
	{
		alignmentCode_ = (alignmentCode_ == 0x7F)? 1 : alignmentCode_+1;
		alignmentID_++;
		Timestamper timestamper;

		double clockBack = getEngine()->generateEvent(alignmentCode_);	//It is crucial that we put this line before
																		//In order to get the Event Timestamp right.
		double timestamp = timestamper.stampSec()-clockBack;

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
		Q_ASSERT(directorData_);
		QByteArray directorDataXml(directorData_->toXml().toLatin1());
		updateCommand->setContent(directorDataXml);
		updateCommand->setFieldValue("Content-Length",QString::number(directorDataXml.length()));
		dataChannel->sendCommand(updateCommand);
	}
}

/*! \brief Called when experimental elements programatically request that a pause be triggered.  
 *	\details Updates the current ComponentStatusManager status (setStatus()) and tells the
 *	PictoEngine to pause.
 *	\sa Picto::Engine::PictoEngine::pauseRequested()
 */
void DirectorStatusManager::pauseRequested()
{
	setStatus(paused);
	getEngine()->pause();
}