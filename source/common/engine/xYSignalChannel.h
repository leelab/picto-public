#ifndef _XYSIGNALCHANNEL_H
#define _XYSIGNALCHANNEL_H

#include <QTimer>
#include <QWidget>
#include <QSharedPointer>

#include "../common.h"
#include "DoubletSignalChannel.h"



namespace Picto {

/*! \brief A signal channel for representing a two dimensional (x,y) position.
 *	\details This is the signal channel used for the test subjects position input.
 *	It includes "x" and "y" sub-channels.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API XYSignalChannel : public DoubletSignalChannel
#else
class XYSignalChannel : public DoubletSignalChannel
#endif
{
	Q_OBJECT

public:
	XYSignalChannel(QString name,QSharedPointer<InputPort> port = QSharedPointer<InputPort>());
	XYSignalChannel(QString name,int xChan, int yChan, int msPerSample,QSharedPointer<InputPort> port = QSharedPointer<InputPort>());

	virtual ~XYSignalChannel() {};
};

};

#endif