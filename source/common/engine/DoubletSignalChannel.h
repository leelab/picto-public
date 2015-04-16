#ifndef _DOUBLETSIGNALCHANNEL_H_
#define _DOUBLETSIGNALCHANNEL_H_

#include <QMap>
#include <QString>
#include <QList>
#include <QObject>

#include "../storage/BehavioralDataUnitPackage.h"
#include "../engine/inputport.h"
#include "../common.h"
#include "SignalChannel.h"

namespace Picto {

/*! \brief DoubletSignalChannel is a base class for SignalChannel objects that are comprised of a doublet (x,y) and scaled.
 *	These objects are used for collecting signals from any device that might be generating data.  
 *
 *	\details DoubletSignalChannels represent logical signals.  Whereas a Picto system might have only one InputPort handling
 *	a single DAQ hardware device, there might be a number of different SignalChannels all gathering data from different
 *	InputPort pins.  An InputPort is therefore passed in as a parameter during SignalChannel construction.
 *
 *	Example DoubletSignalChannels:
 *		- Position - transmits x,y coordinates for input position
 *		- Diameter - transmits x,y dimensions of Pupil diameter
 *
 *	With regards to calibration, the assumpution is that the data may need to undergo a linear calibration
 *	(for example, eyeTracker data needs to be converted from voltages to screen coordinates).  As such, all
 *	DoubletSignalChannels have calibration constants that can be set.  Logic is included to support shear cases as well,
 *	where one coordinate's multiplication factor may be a function to some extent of its other coordinate.
 *	
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API DoubletSignalChannel : public SignalChannel
#else
class DoubletSignalChannel : public SignalChannel
#endif
{
	Q_OBJECT

public:
	DoubletSignalChannel(QString name, QSharedPointer<InputPort> port = QSharedPointer<InputPort>());
	DoubletSignalChannel(QString name, int msPerSample, QSharedPointer<InputPort> port = QSharedPointer<InputPort>());

	virtual ~DoubletSignalChannel() {};

	virtual void addSubchannel(QString subchannelName, int channelIndex);

	void setCalibrationCoefficientsFromRange(QString subchannel, double minRawValue, double maxRawValue, double minScaledValue, double maxScaledValue);
	void setCalibrationCoefficients(QString subchannel, double gain, int offset, double scaledCenterValue);
	void setShear(QString subchannel, QString asFuncOfSubChannel, double shearFactor);

	virtual QMap<QString, QVector<double>> getValues();
	virtual double peekValue(QString subchannel);

	QSharedPointer<BehavioralDataUnitPackage> getDataPackage();

protected:
	//! Flag indicates whether the input values should be scaled and shorn
	bool useScaleFactors_;

private:
	/*! \brief Holds the scale factors needed used in converting the signal coming in on the InputPort to
	 *	the clean SignalChannel data output from this object.
	 *	\details The InputValue can be offset, scaled by a factor and sheared as a function of another sub-channel.  
	 */
	struct scaleFactors
	{
		scaleFactors(){scaleA = 0;scaleB = 1;shearFactor = 0;shearAsFuncOf = "";centerVal = 0;};
		//! A linear offset of the InputPort values.
		double scaleA;
		//! A scale factor by which InputPort values will be multiplied.
		double scaleB;
		/*!	\brief A scale factor by which the shearAsFuncOf signal's offset from its center values will be multiplied and added
		 *	to this value.
		 */
		double shearFactor;
		//! The name of the sub-channel by which this channel will be sheared.
		QString shearAsFuncOf;
		//! The center value of this sub-channel for the purpose of shearing other sub-channels
		double centerVal;
	};

	//! A Map of scaleFactors by channel name.
	QMap<QString, scaleFactors> scaleFactorsMap_;
};


}; //namespace Picto

#endif
