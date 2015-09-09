#ifndef _ANALYSISFRAMEDATA_H_
#define _ANALYSISFRAMEDATA_H_

#include <QWeakPointer>
#include "../common.h"

#include "AnalysisDataSource.h"

namespace Picto {

/*!	\brief An AnalysisDataSource for gathering frame timing data.
 *	\details This object can be used to query the latest, next and previous
 *	frame times as well as lists of frame times for input time ranges.
 *	
 *	All frame times are the Picto system's best estimate for the time at which the first 
 *	phosphor of the frame appeared on screen.  When the frame is displayed with DirectX
 *	as in the Director, this estimate is fairly precise (within a few ms) since DirectX
 *	can be polled for that type of information.  When using Pixmap rendering, the value
 *	is less precise.
 *	\note This class uses an LfpReader as its main data source.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisFrameData : public AnalysisDataSource
#else
class AnalysisFrameData : public AnalysisDataSource
#endif
{
	Q_OBJECT
public:
	AnalysisFrameData();
	virtual ~AnalysisFrameData(){};

	static QSharedPointer<Asset> Create();

	virtual void enteredScope();

	virtual QString friendlyTypeName(){return "Frame Data";};
	virtual QString getUITemplate(){return "FrameDataSource";};
	virtual QString getUIGroup(){return "Sensors";};

public slots:
	void zeroLatestFrame();
	double getZeroTime() const;
	double getLatestTime();
	double getPrevTime();
	double getNextTime();
	QVariantList getPrevTimes(double secsPreceding);
	QVariantList getNextTimes(double secsFollowing);
	QVariantList getTimesSince(double beyondTime);
	QVariantList getTimesUntil(double upToTime);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	double getLatestRunTime();
	double zeroTime_;


};


}; //namespace Picto

#endif
