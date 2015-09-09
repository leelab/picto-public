#ifndef _AnalysisLfpData_H_
#define _AnalysisLfpData_H_

#include <QWeakPointer>
#include "../common.h"

#include "AnalysisDataSource.h"

namespace Picto {

/*!	\brief An AnalysisDataSource for gathering lfp data.
 *	\details This object can be used to query local field potential values as well as metadata (the channels that were
 *	recorded in the current run, the sample period etc.)
 *	\note For test runs in the testviewer, the data returned from this object will necessarily be fake sample data.
 *	For consistency's sake, any functions requesting future data will return meaningless values.
 *	\note This class uses a FrameReader object as its main data source.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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

	virtual void enteredScope();

	virtual QString friendlyTypeName(){return "Lfp Data";};
	virtual QString getUITemplate(){return "LFPDataSource";};
	virtual QString getUIGroup(){return "Sensors";};

public slots:
	void zeroLatestFrame();
	double getZeroTime() const;
	virtual QVariantList getChannels();
	double getSamplePeriod();
	double getLatestTime();
	double getLatestValue(int channel);
	QVariantList getLatestValue();
	double getNextTime();
	double getNextValue(int channel);
	QVariantList getNextValue();
	QVariantList getPrevTimes(double secsPreceding);
	QVariantList getNextTimes(double secsFollowing);
	QVariantList getTimesSince(double beyondTime);
	QVariantList getTimesUntil(double upToTime);
	QVariantList getPrevValues(int channel,double secsPreceding);
	QVariantList getNextValues(int channel,double secsFollowing);
	QVariantList getPrevValues(double secsPreceding);
	QVariantList getNextValues(double secsFollowing);
	QVariantList getValuesSince(int channel,double beyondTime);
	QVariantList getValuesUntil(int channel,double upToTime);
	QVariantList getValuesSince(double beyondTime);
	QVariantList getValuesUntil(double upToTime);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	double getLatestRunTime();
	double zeroTime_;
	QSharedPointer<LfpReader> lfpReader_;


};


}; //namespace Picto

#endif
