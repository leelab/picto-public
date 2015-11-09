#ifndef _ANALYSISREWARDDATA_H_
#define _ANALYSISREWARDDATA_H_

#include <QWeakPointer>
#include "../common.h"

#include "AnalysisDataSource.h"

namespace Picto {

/*!	\brief An AnalysisDataSource for gathering reward timing data.
 *	\details This object can be used to query reward onset times as well as their durations.
 *	\note This class uses a RewardReader as its main data source.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	void zeroLatestFrame();
	double getZeroTime() const;
	double getLatestTime();
	double getNextTime();
	int getLatestDuration();
	int getNextDuration();
	QVariantList getPrevTimes(double secsPreceding);
	QVariantList getNextTimes(double secsFollowing);
	QVariantList getTimesSince(double beyondTime);
	QVariantList getTimesUntil(double upToTime);
	QVariantList getPrevDurations(double secsPreceding);
	QVariantList getNextDurations(double secsFollowing);
	QVariantList getDurationsSince(double beyondTime);
	QVariantList getDurationsUntil(double upToTime);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	double getLatestRunTime();
	double zeroTime_;


};


}; //namespace Picto

#endif
