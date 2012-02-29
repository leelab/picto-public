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

/*! SignalChannel
 *
 *	SignalChannel is a base class for signalChannel objects
 *	These objects will be used for collecting signals from any
 *	device that might be generating data.  
 *
 *	Data
 *	SignalChannels will store and transmit all data as doubles.  This is done since 
 *	it is the most flexible , however the user will need to be aware of this when 
 *	using values that are clearly integers (like mouse coordinate).  The data is sampled
 *	at a fixed rate and stored in a FIFO.  Each Signal Channel has  a fixed (but aribtrary)
 *	number of subchannels.  So, a mouse channel would probably have 2 subchannels (x & y), 
 *	while a PictBox Analog	Input channel, would have 4 subchannels (eye tracker x&y, 
 *	pupil width & height).
 *
 *	Example SignalChannels:
 *		MouseCoordSignalChannel - transmits a single coordinate from the mouse
 *		PictoBoxAnalogInputSignalChannel - transmits an AI cahnnel from the PictoBox
 *
 *	Calibration
 *	The assumpution is that the data may need to undergo a linear calibration
 *	(for example, eyeTracker data needs to be converted from voltages to screen 
 *	coordinates).  As such, all SignalChannels have calibration constants that 
 *	can be set.
 *
 *	NOTE: If a SignalChannel is going to be used for x/y data, it must contain
 *	xpos and ypos subchannels.
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

	void addSubchannel(QString subchannelName, int channelIndex);
	void setSampleResolution(int msPerSample);
	void setCalibrationCoefficientsFromRange(QString subchannel, double minRawValue, double maxRawValue, double minScaledValue, double maxScaledValue);
	void setCalibrationCoefficients(QString subchannel, double gain, int offset, double scaledCenterValue);
	void setShear(QString subchannel, QString asFuncOfSubChannel, double shearFactor);
	QList<QString> getSubchannels() { return rawDataBuffer_.keys(); };
	double latestUpdateEventOffset();

	bool start();
	bool stop();

	QMap<QString, QVector<double>> getValues();
	void updateData(double currentTime);
	QMap<QString, QVector<double>> getRawValues();
	QSharedPointer<BehavioralDataUnitPackage> getDataPackage();
	QString getName(){return name_;};
	QString getPortType(){return port_?port_->type():"";};

	double peekValue(QString subchannel);
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
	bool useScaleFactors_;
	QString name_;

private:
	double scaleData(int subchannel, double rawData);
	
	struct scaleFactors
	{
		scaleFactors(){scaleA = 0;scaleB = 1;shearFactor = 0;shearAsFuncOf = "";centerVal = 0;};
		double scaleA;
		double scaleB;
		double shearFactor;
		QString shearAsFuncOf;
		double centerVal;
	};

	QMap<QString, scaleFactors> scaleFactorsMap_;
	QMap<QString, int> channelIndexMap_;
	QSharedPointer<InputPort> port_;

};


}; //namespace Picto

#endif
