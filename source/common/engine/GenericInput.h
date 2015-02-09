#ifndef _GENERICINPUT_H
#define _GENERICINPUT_H

#include <QTimer>
#include <QWidget>
#include <QSharedPointer>

#include "../common.h"
#include "SignalChannel.h"

namespace Picto{

	/*! \brief A signal channel for representing Generic Inputs to be read by the State Machine.
	*	\details This is the signal channel used for any analog inputs unitilzed by an Experiment.
	*	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
	*	\date 2009-2015
	*/
#if defined WIN32 || defined WINCE
class PICTOLIB_API GenericInput : public SignalChannel
#else
class XYSignalChannel : public SignalChannel
#endif
{
	Q_OBJECT
public:
	GenericInput(QString name, QSharedPointer<InputPort> port = QSharedPointer<InputPort>());
	GenericInput(QVector<int> &qvUsedChannels, QString name, QSharedPointer<InputPort> port = QSharedPointer<InputPort>());
};

};

#endif