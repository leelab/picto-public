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
 *	\details SignalChannels represent logical signals.  Whereas a Picto system might have only one InputPort handling a single DAQ
 *	hardware device, there might be a number of different SignalChannels all gathering data from different InputPort pins.
 *	An InputPort is therefore passed in as a parameter during SignalChannel construction.
 *	
 *	SignalChannels store and transmit all data as doubles.  This is done since 
 *	it is the most flexible , however the user will need to be aware of this when 
 *	using values that are clearly integers (like mouse coordinate).  The data is sampled
 *	at a fixed rate and stored in a FIFO.  Each Signal Channel has a fixed (but arbitrary)
 *	number of subchannels.  So, a position channel would probably have 2 subchannels (x & y), 
 *	while another PictoBox Analog Input channel might have some other number of subchannels
 *
 *	Example SignalChannels:
 *		- Position - transmits x,y coordinates for input position
 *		- Diameter - transmits x,y dimensions of Pupil diameter
 *
 *	With regards to calibration, the assumpution is that the data may need to undergo a linear calibration
 *	(for example, eyeTracker data needs to be converted from voltages to screen 
 *	coordinates).  As such, all SignalChannels have calibration constants that 
 *	can be set.  Logic is included to support shear cases as well, where one coordinate's multiplication factor
 *	may be a function to some extent of its other coordinate.
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

	/*! \brief Returns a list of the names of this SignalChanne''s sub-channels.*/
	QList<QString> getSubchannels() { return rawDataBuffer_.keys(); };
	double latestUpdateEventOffset();

	bool start();
	bool stop();

	virtual QMap<QString, QVector<double>> getValues();
	void updateData(double currentTime);
	virtual QMap<QString, QVector<double>> getRawValues();

	/*! \brief Returns the name of this SignalChannel.*/
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
	//This function places current data (everything that has been generated
	//since the last call) into rawDataBuffer_.  The buffer should not be assumed
	//to be empty (nor should it be emptied), since an insertValue call may have 
	//added data.
	void getDataFromPort();

	QMap<QString, QVector<double> > rawDataBuffer_;
	QMap<QString, double > rawDataLastValue_;

	int msPerSample_;			//samples per second collected by the channel
	QString name_;

	QMap<QString, int> channelIndexMap_;
	QSharedPointer<InputPort> port_;

};


}; //namespace Picto

#endif
