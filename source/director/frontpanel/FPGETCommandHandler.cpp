#include <QByteArray>
#include <QStringList>

#include <QTextStream>

#include "FPInterface.h"
#include "FPGETCommandHandler.h"
#include "../../common/globals.h"


FPGETCommandHandler::FPGETCommandHandler()
{
}

/*! \brief Parses the input ProtocolCommand to check its syntax and return any requested information.
 *	\details Available data includes IP Address, PictoBox Name, Reward duration, Flush duration, status string.
 */
QSharedPointer<Picto::ProtocolResponse> FPGETCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	QTextStream out(stdout);

	QSharedPointer<Picto::ProtocolResponse> response(
		new Picto::ProtocolResponse(Picto::Names->directorAppName,
									"PICTO",
									"1.0",
									Picto::ProtocolResponseType::OK));
	response->setFieldValue("Command-ID",command->getFieldValue("Command-ID"));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(
		new Picto::ProtocolResponse(Picto::Names->directorAppName,
									"PICTO",
									"1.0",
									Picto::ProtocolResponseType::NotFound));
	notFoundResponse->setFieldValue("Command-ID",command->getFieldValue("Command-ID"));

	QByteArray content;

	if(command->getTarget() == "/IP")
	{
		content = ip_.toUtf8();
	}
	else if(command->getTarget() == "/boxname")
	{
		content = name_.toUtf8();
	}
	else if(command->getTarget().startsWith("/reward"))
	{
		QStringList targetFields = command->getTarget().split('/',QString::SkipEmptyParts);
		if(targetFields.size()>2 && targetFields[2] == "duration")
		{
			bool ok;
			int controller = targetFields[1].toInt(&ok);
			if(ok)
				content = QString("%1").arg(getRewardDuration(controller)).toUtf8();
			else
			{
				out<<"Reward controller not an int\n";
				return notFoundResponse;
			}
		}
		else if(targetFields.size()>2 && targetFields[2] == "flushduration")
		{
			bool ok;
			int controller = targetFields[1].toInt(&ok);
			if(ok)
				content = QString("%1").arg(getFlushDuration(controller)).toUtf8();
			else
			{
				out<<"Reward controller not an int\n";
				return notFoundResponse;
			}
		}
	//	//else if(targetFields.size()>2 && targetFields[2] == "flushtimeremain")
	//	//{
	//	//	content = QString("%1").arg(eng->getFlushTimeRemain()).toUtf8();
	//	//}
	//	else
	//	{
	//		out<<"Bad split.\n";
	//		return notFoundResponse;
	//	}
	}
	else if(command->getTarget() == "/status")
	{
		content = status_.toUtf8();
	}
	else
	{
		return notFoundResponse;
	}

	response->setContent(content);

	return response;
}

/*! \brief Use this function to tell this object what the current reward duration is so that it can be correctly returned to
 *	anyone requesting that data.
 */
void FPGETCommandHandler::setRewardDuration(int controller,int dur)
{
	while(rewardDurs_.size() <= controller)
		rewardDurs_.append(100);
	rewardDurs_[controller] = dur;
}

/*! \brief Use this function to tell this object what the current flush duration is so that it can be correctly returned to
 *	anyone requesting that data.
 */
void FPGETCommandHandler::setFlushDuration(int controller,int dur)
{
	while(flushDurs_.size() <= controller)
		flushDurs_.append(100);
	flushDurs_[controller] = dur;
}

/*! \brief Returns the reward duration that was set using setRewardDuration().
 */
int FPGETCommandHandler::getRewardDuration(int controller)
{
	if(rewardDurs_.size() <= controller)
		return -1;
	return rewardDurs_[controller];
}

/*! \brief Returns the flush duration that was set using setFlushDuration().
 */
int FPGETCommandHandler::getFlushDuration(int controller)
{
	if(flushDurs_.size() <= controller)
		return -1;
	return flushDurs_[controller];
}