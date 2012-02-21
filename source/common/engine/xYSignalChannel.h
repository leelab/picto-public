#ifndef _XYSIGNALCHANNEL_H
#define _XYSIGNALCHANNEL_H

#include <QTimer>
#include <QWidget>
#include <QSharedPointer>

#include "../common.h"
#include "SignalChannel.h"



namespace Picto {

/*! \brief A signal channel for representing an XY point.
 *
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API XYSignalChannel : public SignalChannel
#else
class XYSignalChannel : public SignalChannel
#endif
{
	Q_OBJECT

public:
	XYSignalChannel(QString name);
	XYSignalChannel(QString name, int sampsPerSecond);

	bool start(){return true;};
	bool stop(){return true;};
	void updateDataBuffer(){};


};

};

#endif