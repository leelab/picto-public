#ifndef _ANALYSISFRAMEDATA_H_
#define _ANALYSISFRAMEDATA_H_

#include <QWeakPointer>
#include "../common.h"

#include "AnalysisDataSource.h"

namespace Picto {

/*!	\brief A parameter for retrieving frame times.
 *
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

	virtual void reset();

	virtual QString friendlyTypeName(){return "Frame Data";};
	virtual QString getUITemplate(){return "Sensor";};
	virtual QString getUIGroup(){return "Sensors";};

public slots:
	//Sets the time of the latest frame to zero.  Other frame times will be returned with respect to
	//this latest frame.
	void zeroLatestFrame();
	//Returns the time of the latest frame to have been shown
	double getLatestTime();
	//Returns the time of the frame shown before the latest one
	double getPrevTime();
	//Returns the time of the next frame to be shown.
	double getNextTime();
	//Returns a list of frame times that occured with times > the input # sec before the latest frame and <= the latest frame time.
	QVariantList getPrevTimes(double secsPreceding);
	//Returns a list of frame times that occured with times <= the input # sec after the latest frame and > the latest frame time.
	QVariantList getNextTimes(double secsFollowing);

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
