#ifndef _SIGNALCHANNEL_H_
#define _SIGNALCHANNEL_H_

#include <QMap>
#include <QString>
#include <QList>
#include <QObject>

#include "../storage/BehavioralDataUnitPackage.h"
#include "../engine/inputport.h"
#include "../common.h"

namespace Picto {

/*! \brief SignalChannel is a base class for signalChannel objects.
 *	These objects are used for collecting signals from any
 *	device that might be generating data.  
 *
 *	\details SignalChannels represent logical signals.  Whereas a Picto system might have only one InputPort handling a
 *	single DAQ hardware device, there might be a number of different SignalChannels all gathering data from different
 *	InputPort pins.  An InputPort is therefore passed in as a parameter during SignalChannel construction.
 *	
 *	SignalChannels store and transmit all data as doubles.  This is done since it is the most flexible , however the user
 *	will need to be aware of this when using values that are clearly integers (like mouse coordinate).  The data is sampled
 *	at a fixed rate and stored in a FIFO.  Each SignalChannel can have an arbitrary number of subchannels.
 *	So, a position channel would probably have 2 subchannels (x & y), while another PictoBox Analog Input channel might
 *	have some other number of subchannels
 *
 *	Example SignalChannels:
 *		- Position - transmits x,y coordinates for input position
 *		- Diameter - transmits x,y dimensions of Pupil diameter
 *	
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API SignalChannel : public QObject
#else
class SignalChannel : public QObject
#endif
{
	Q_OBJECT

public:
	SignalChannel(QString name,QSharedPointer<InputPort> port = QSharedPointer<InputPort>());
	SignalChannel(QString name,int msPerSample,QSharedPointer<InputPort> port = QSharedPointer<InputPort>());

	virtual ~SignalChannel() {};

	virtual void addSubchannel(QString subchannelName, int channelIndex);
	void setSampleResolution(int msPerSample);

	//! Returns a list of the names of this SignalChanne''s sub-channels.
	QList<QString> getSubchannels() { return rawDataBuffer_.keys(); };
	double latestUpdateEventOffset();

	bool start();
	bool stop();

	virtual QMap<QString, QVector<double>> getValues();
	void updateData(double currentTime);
	virtual QMap<QString, QVector<double>> getRawValues();

	//! Returns the name of this SignalChannel.
	QString getName(){return name_;};
	/*! \brief Returns a string describing the type of this SignalChannel's underlying InputPort.  
	 *	\details For example, a MouseInputPort would return "mouse".
	 */
	QString getPortType(){return port_?port_->type():"";};

	virtual double peekValue(QString subchannel);
	void clearValues();

	void insertValue(QString subchannel, double val);
	void insertValues(QString subchannel, QVector<double> vals);

protected:
	/*!	\brief Places current data (everything that has been generated since the last call) into rawDataBuffer_.
	 *	\note The buffer should not be assumed to be empty (nor should it be emptied), since an insertValue call may have
	 *	added data.
	 */
	void getDataFromPort();

	//! Holds data buffers indexed by channel name
	QMap<QString, QVector<double> > rawDataBuffer_;
	//! Holds the last data element, used for peeking.  Indexed by channel name.
	QMap<QString, double > rawDataLastValue_;

	//! Sample period in milliseconds
	int msPerSample_;
	//! The name of this signal channel, passed in through the constructor
	QString name_;

	//! A map of all this SignalChannel object's channel numbers, indexed by channel name.
	QMap<QString, int> channelIndexMap_;
	//! A shared pointer to this SignalChannel's InputPort object, used to actually collect data.
	QSharedPointer<InputPort> port_;

};


}; //namespace Picto

#endif
