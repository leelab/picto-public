#ifndef _NETWORK_SIGNAL_CHANNEL_H
#define _NETWORK_SIGNAL_CHANNEL_H

#include <QSharedPointer>

#include "../common.h"
#include "SignalChannel.h"
#include "../network/commandchannel.h"



namespace Picto {

/*! \brief A signal channel that pulls input data off the network
 *
 *	If we're remotely viewing an experiment, we will need to get the positional
 *	data from the network using GETDATA commands.  This signal channel does that.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API NetworkSignalChannel : public SignalChannel
#else
class NetworkSignalChannel : public SignalChannel
#endif
{
	Q_OBJECT

public:
	NetworkSignalChannel(CommandChannel *serverChannel);
	~NetworkSignalChannel() {};

	bool start();
	bool stop();
	void updateDataBuffer();

	void setLastTimeDataCollected(QString time) { lastTimeDataCollected_ = time; };

private:
	CommandChannel *serverChannel_;
	QString lastTimeDataCollected_;
};

};

#endif