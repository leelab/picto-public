#ifndef _SIGNALVALUEPARAMETER_H_
#define _SIGNALVALUEPARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*! \brief A Parameter that can be used to capture the latest values of an input signal.
 *
 *	This parameter can be used to capture the value of an input signal.
 *	Values returned by this object are the latest available at the time that the latest
 *	frame started.  Values can be polled using the getValue() script function.  The signal
 *	to be tracked is set in the Signal Property, where the default value is the "Position" 
 *	channel.
 *	\note There is a design error in the way that SignalValueParameters are updated with new signal data.  It is the same
 *	problem that is described in the OperatorClickParameter, so if this class is fixed, that one should be looked at too.
 *	Currently, SignalValueParameter objects get their signal data from a static list that is loaded whenever setLatestValue()
 *	is called by the Engine::PictoEngine.  The problem here is that we cannot be sure that two separate Experiments won't be 
 *	open and running at the same time in the same application.  In this case, Signal data from one Experiment would end 
 *	up being delivered to SignalValueParameters in another.  We dealt with this kind of issue in the FrameResolutionTimer by 
 *	creating a FrameTracker class, such that each experiment has a single FrameTracker object that is passed into all FrameResolutionTimer 
 *	objects in a single experiment.  Each Experiment's FrameTracker can then be updated separately, and its updates be passed to only 
 *	the FrameResolutionTimers in its own experiment.  We should really set something like this up for the SignalValueParameter, but in the 
 *	meantime the SignalValueParameter is really only used for calibration purposes and has not been "mission critical" so 
 *	the current system will work for now.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API SignalValueParameter : public Parameter
#else
class SignalValueParameter : public Parameter
#endif
{
	Q_OBJECT

public:

	static QSharedPointer<Asset> Create();
	virtual ~SignalValueParameter(){};
	static void setLatestValue(QString signal, QString subChannel, double value);

	virtual QString friendlyTypeName(){return "Signal Value";};
	virtual QString getUITemplate(){return "Sensor";};
	virtual QString getUIGroup(){return "Sensors";};

public slots:
	double getValue(QString subChannel = "");

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	SignalValueParameter();
	static QHash<QString,QHash<QString,double>> signalData_;

};


}; //namespace Picto

#endif
