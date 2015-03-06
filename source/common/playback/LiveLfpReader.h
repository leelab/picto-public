#ifndef _LiveLfpReader_H_
#define _LiveLfpReader_H_
#include <QVector>
#include <QHash>
#include "../../common/playback/PlaybackInterfaces.h"

namespace Picto {
/*! \brief Implements the LfpReader class for a live test experiment.
 *	\details Since this is meant to be used with a test experiment, this object just generates fake LFP
 *	data.  It returns lfp values based on a sine wave function of sample times.  The sample times to be
 *	used are input using the createVirtualLfpData() function.  Since this object is filled during run time, 
 *	Analysis have access to all past data but no future data.  Requests for future data from this class will result in 
 *	meaningless values.
 *	\note Since the functions here simply implement the LfpReader class for
 *	live session data, there is not much to add in terms of documentation 
 *	beyond what was described above, so we will not be adding additional function level documentation
 *	for many of the functions in this class.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API LiveLfpReader : public LfpReader
#else
class LiveLfpReader : public LfpReader
#endif
{
	Q_OBJECT
public:
	LiveLfpReader(QList<int> channels,double samplePeriod);
	void setRunStart();
	void createVirtualLfpData(double time);
	void setRunEnd();

	//Lfp Reader Interface
	//Returns the available lfp channels
	virtual QVariantList getChannels();
	virtual double getSamplePeriod();
	virtual double getLatestTime();
	virtual double getLatestValue(int channel);
	virtual double getNextTime();
	virtual double getNextValue(int channel);
	//Returns signal values for the input lfp channel with times > the input time.
	virtual QVariantList getValuesSince(int channel,double time);
	virtual QVariantList getValuesUntil(int channel,double time);
	virtual QVariantList getTimesSince(double time);
	virtual QVariantList getTimesUntil(double time);

private:
	QVector<double> periods_;
	QVector<double> amplitudes_;
	float getValue(int channel, double time);
	QVector<double> data_;
	QHash<int,int> chanIndexMap_;
	QVariantList channels_;
	int numChannels_;
	double samplePeriod_;
};

}; //namespace Picto
#endif