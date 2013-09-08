#ifndef _AnalysisLfpData_H_
#define _AnalysisLfpData_H_

#include <QWeakPointer>
#include "../common.h"

#include "AnalysisDataSource.h"

namespace Picto {

/*!	\brief A parameter for retrieving frame times.
 *
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisLfpData : public AnalysisDataSource
#else
class AnalysisLfpData : public AnalysisDataSource
#endif
{
	Q_OBJECT
public:
	AnalysisLfpData();
	virtual ~AnalysisLfpData(){};

	static QSharedPointer<Asset> Create();

	virtual void reset();

	virtual QString friendlyTypeName(){return "Lfp Data";};
	virtual QString getUITemplate(){return "LFPDataSource";};
	virtual QString getUIGroup(){return "Sensors";};

public slots:
	//Sets the time of the latest frame to zero.  Lfp times will be returned with respect to
	//this latest frame.
	void zeroLatestFrame();
	//Returns a list of active lfp channel numbers
	virtual QVariantList getChannels();
	//Returns the lfp sample period.
	double getSamplePeriod();
	//Returns the time of the latest lfp reading
	double getLatestTime();
	//Returns the latest value of the input lfp channel
	double getLatestValue(int channel);
	//Returns an array of the latest lfp values ordered according to the channel order returned from getChannels()
	QVariantList getLatestValue();
	//Returns the time of the next lfp reading
	double getNextTime();
	//Returns the next value that will be read for the input lfp channel
	double getNextValue(int channel);
	//Returns an array of the next lfp values to be read ordered according to the order returned from getChannels()
	QVariantList getNextValue();
	//Returns a list of lfp read times that occured with times > the input # sec before the latest frame and <= the latest frame time
	QVariantList getPrevTimes(double secsPreceding);
	//Returns a list of lfp read times that will occur with times > the latest frame time and <= the input # sec after the latest frame
	QVariantList getNextTimes(double secsFollowing);
	//Returns a list of lfp values for the input lfp channel that occured with times > the input # sec before the latest frame and <= the latest frame time
	QVariantList getPrevValues(int channel,double secsPreceding);
	//Returns a list of lfp values for the input lfp channel that will occur with times > the latest frame time and <= the input # sec after the latest frame
	QVariantList getNextValues(int channel,double secsFollowing);
	//Returns a list of signal values for all lfp channels (ordered like the result of getChannels())
	//that will occur with times > the input # sec before the latest frame time and <= the latest 
	//frame time. Returned value is a list of lists such that each index of the list contains a list of lfp channel values
	//ordered like the result of getChannels().  Times for these values can be read in using getPrevTimes() with the same 
	//input.
	QVariantList getPrevValues(double secsPreceding);
	//Returns a list of lfp values for all lfp channels that will occur with times > the latest frame 
	//time and <= the input # sec after the latest frame. Returned value is a list of lists such that each 
	//index of the list contains a list of lfp channel values ordered like the result of getChannels(). 
	//Times for these values can be read in using getNextTimes() with the same input.
	QVariantList getNextValues(double secsFollowing);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	//Returns the latest frame time with respect to the beginning of the run
	double getLatestRunTime();
	double zeroTime_;
	QSharedPointer<LfpReader> lfpReader_;


};


}; //namespace Picto

#endif
