#ifndef _AnalysisSpikeData_H_
#define _AnalysisSpikeData_H_

#include <QWeakPointer>
#include "../common.h"

#include "AnalysisDataSource.h"

namespace Picto {

/*!	\brief An AnalysisDataSource for gathering spike data.
 *	\details This object can be used to query spike times, channels, units and waveforms as well as metadata
 *	(the sample period of the spike acquisition system).
 *	
 *	A spike's channel is the electrode number on which is was detected.  The unit is the index of the spike detection
 *	filter being used on that electrode for cases where multiple spikes are detected on one electrode.
 *	
 *	\note For test runs in the testviewer, the data returned from this object will necessarily be fake sample data.
 *	For consistency's sake, any functions requesting future data will return meaningless values.
 *	\note This class uses a SpikeReader as its main data source.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API AnalysisSpikeData : public AnalysisDataSource
#else
class AnalysisSpikeData : public AnalysisDataSource
#endif
{
	Q_OBJECT
public:
	AnalysisSpikeData();
	virtual ~AnalysisSpikeData(){};

	static QSharedPointer<Asset> Create();

	virtual void enteredScope();

	virtual QString friendlyTypeName(){return "Spike Data";};
	virtual QString getUITemplate(){return "SpikeDataSource";};
	virtual QString getUIGroup(){return "Sensors";};

public slots:
	void zeroLatestFrame();
	double getSamplePeriod();
	double getLatestTime();
	int getLatestChannel();
	int getLatestUnit();
	QVariantList getLatestWaveform();
	double getNextTime();
	int getNextChannel();
	int getNextUnit();
	QVariantList getNextWaveform();
	QVariantList getPrevTimes(double secsPreceding);
	QVariantList getPrevChannels(double secsPreceding);
	QVariantList getPrevUnits(double secsPreceding);
	QVariantList getPrevWaveforms(double secsPreceding);
	QVariantList getNextTimes(double secsFollowing);
	QVariantList getNextChannels(double secsFollowing);
	QVariantList getNextUnits(double secsFollowing);
	QVariantList getNextWaveforms(double secsFollowing);
	QVariantList getTimesSince(double beyondTime);
	QVariantList getTimesUntil(double upToTime);
	QVariantList getChannelsSince(double beyondTime);
	QVariantList getChannelsUntil(double upToTime);
	QVariantList getUnitsSince(double beyondTime);
	QVariantList getUnitsUntil(double upToTime);
	QVariantList getWaveformsSince(double beyondTime);
	QVariantList getWaveformsUntil(double upToTime);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	double getLatestRunTime();
	double zeroTime_;


};


}; //namespace Picto

#endif
