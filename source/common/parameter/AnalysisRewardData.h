#ifndef _ANALYSISREWARDDATA_H_
#define _ANALYSISREWARDDATA_H_

#include <QWeakPointer>
#include "../common.h"

#include "AnalysisDataSource.h"

namespace Picto {

/*!	\brief A parameter for retrieving frame times.
 *
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisRewardData : public AnalysisDataSource
#else
class AnalysisRewardData : public AnalysisDataSource
#endif
{
	Q_OBJECT
public:
	AnalysisRewardData();
	virtual ~AnalysisRewardData(){};

	static QSharedPointer<Asset> Create();

	virtual void enteredScope();

	virtual QString friendlyTypeName(){return "Reward Data";};
	virtual QString getUITemplate(){return "RewardDataSource";};
	virtual QString getUIGroup(){return "Sensors";};

public slots:
	//Sets the time of the latest frame to zero.  Reward times will be returned with respect to
	//this latest frame.
	void zeroLatestFrame();
	//Returns the time of the latest reward to have been supplied
	double getLatestTime();
	//Returns the time of the next reward to be supplied.
	double getNextTime();
	//Returns the duration of the latest reward to have been supplied
	int getLatestDuration();
	//Returns the duration of the next reward to be supplied.
	int getNextDuration();
	//Returns a list of reward times that occured with times > the input # sec before the latest frame and <= the latest frame time.
	QVariantList getPrevTimes(double secsPreceding);
	//Returns a list of reward times that occured with times <= the input # sec after the latest frame and > the latest frame time.
	QVariantList getNextTimes(double secsFollowing);
	//Functions like getPrevTimes except that the input time is an absolute time with respect to this element's zero time instead of an offset
	QVariantList getTimesSince(double beyondTime);
	//Functions like getNextTimes except that the input time is an absolute time with respect to this element's zero time instead of an offset
	QVariantList getTimesUntil(double upToTime);
	//Returns a list of reward durations that occured with times > the input # sec before the latest frame and <= the latest frame time.
	QVariantList getPrevDurations(double secsPreceding);
	//Returns a list of reward durations that occured with times <= the input # sec after the latest frame and > the latest frame time.
	QVariantList getNextDurations(double secsFollowing);
	//Functions like getPrevDurations except that the input time is an absolute time with respect to this element's zero time instead of an offset
	QVariantList getDurationsSince(double beyondTime);
	//Functions like getNextDurations except that the input time is an absolute time with respect to this element's zero time instead of an offset
	QVariantList getDurationsUntil(double upToTime);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	//Returns the latest frame time with respect to the beginning of the run
	double getLatestRunTime();
	double zeroTime_;


};


}; //namespace Picto

#endif
