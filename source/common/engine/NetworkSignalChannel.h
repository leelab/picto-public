/*! \brief A signal channel that pulls input data off the network
 *
 *	If we're remotely viewing an experiment, we will need to get the positional
 *	data from the network using GETDATA commands.  This signal channel does that.
 */

#ifndef _NETWORK_SIGNAL_CHANNEL_H
#define _NETWORK_SIGNAL_CHANNEL_H

#include <QObject>
#include <QSharedPointer>

#include "../common.h"
#include "SignalChannel.h"
#include "../network/commandchannel.h"



namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API NetworkSignalChannel : public QObject, public SignalChannel
#else
class NetworkSignalChannel :  public QObject, public SignalChannel
#endif
{
	Q_OBJECT

public:
	NetworkSignalChannel(CommandChannel *serverChannel);
	~NetworkSignalChannel() {};

	bool start();
	bool stop();
	void updateDataBuffer();

private:
	CommandChannel *serverChannel_;
	double lastTimeDataCollected_;
};

};

#endif