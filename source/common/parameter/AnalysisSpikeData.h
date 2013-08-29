#ifndef _AnalysisSpikeData_H_
#define _AnalysisSpikeData_H_

#include <QWeakPointer>
#include "../common.h"

#include "AnalysisDataSource.h"

namespace Picto {

/*!	\brief A parameter for retrieving frame times.
 *
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

	virtual void reset();

	virtual QString friendlyTypeName(){return "Spike Data";};
	virtual QString getUITemplate(){return "Sensor";};
	virtual QString getUIGroup(){return "Sensors";};

public slots:
	//Sets the time of the latest frame to zero.  Spike times will be returned with respect to
	//this latest frame.
	void zeroLatestFrame();
	//Returns the time of the latest spike to have been read
	double getLatestTime();
	int getLatestChannel();
	int getLatestUnit();
	QVariantList getLatestWaveform();
	//Returns the time of the next spike to be read.
	double getNextTime();
	int getNextChannel();
	int getNextUnit();
	QVariantList getNextWaveform();
	//Returns a list of spike times that occured with times > the input # sec before the latest frame and <= the latest frame time.
	QVariantList getPrevTimes(double secsPreceding);
	QVariantList getPrevChannels(double secsPreceding);
	QVariantList getPrevUnits(double secsPreceding);
	QVariantList getPrevWaveforms(double secsPreceding);
	//Returns a list of spike times that will occur with times <= the input # sec after the latest frame and > the latest frame time.
	QVariantList getNextTimes(double secsFollowing);
	QVariantList getNextChannels(double secsFollowing);
	QVariantList getNextUnits(double secsFollowing);
	QVariantList getNextWaveforms(double secsFollowing);

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
