#ifndef _AnalysisSignalData_H_
#define _AnalysisSignalData_H_

#include <QWeakPointer>
#include "../common.h"

#include "AnalysisDataSource.h"

namespace Picto {

/*!	\brief A parameter for retrieving frame times.
 *
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisSignalData : public AnalysisDataSource
#else
class AnalysisSignalData : public AnalysisDataSource
#endif
{
	Q_OBJECT
public:
	AnalysisSignalData();
	virtual ~AnalysisSignalData(){};

	static QSharedPointer<Asset> Create();

	virtual void reset();

	virtual QString friendlyTypeName(){return "Signal Data";};
	virtual QString getUITemplate(){return "Sensor";};
	virtual QString getUIGroup(){return "Sensors";};

public slots:
	//Sets the time of the latest frame to zero.  Signal times will be returned with respect to
	//this latest frame.
	void zeroLatestFrame();
	//Returns the component names of the signal, ie. x,y
	virtual QStringList getComponentNames();
	//Returns the sample period for this signal.
	double getSamplePeriod();
	//Returns the time of the latest signal to have been read
	double getLatestTime();
	//Returns the latest value of the input signal component
	double getLatestValue(QString componentName);
	//Returns an array of the latest values ordered according to the sub channel order returned from getComponentNames()
	QVariantList getLatestValue();
	//Returns the time of the next signal to have be read
	double getNextTime();
	//Returns the next value that will be read for the input signal component
	double getNextValue(QString componentName);
	//Returns an array of the next values to be read ordered according to the sub channel order returned from getComponentNames()
	QVariantList getNextValue();
	//Returns a list of signal values for the input sub channel that occured with times > the input # sec before the latest frame and <= the latest frame time
	virtual QVariantList getPrevValues(QString componentName,double secsPreceding);
	//Returns a list of signal values for the input sub channel that will occur with times > the latest frame time and <= the input # sec after the latest frame
	virtual QVariantList getNextValues(QString componentName,double secsFollowing);
	//Returns a list of signal values for all subcomponents (ordered like the result of getComponentNames())
	//that will occur with times > the input # sec before the latest frame time and <= the latest 
	//frame time. Returned value is a list of lists such that each index of the list contains a list signal subcomponents
	//ordered like the result of getComponentNames(). Times should be incremented by one getSamplePeriod() for each index.
	virtual QVariantList getPrevValues(double secsPreceding);
	//Returns a list of signal values for all subcomponents that will occur with times > the latest frame 
	//time and <= the input # sec after the latest frame. Returned value is a list of lists such that each 
	//index of the list contains a list signal subcomponents ordered like the result of getComponentNames(). 
	//Times should be incremented by one getSamplePeriod() for each index.
	virtual QVariantList getNextValues(double secsFollowing);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	//Returns the latest frame time with respect to the beginning of the run
	double getLatestRunTime();
	double zeroTime_;
	QSharedPointer<SignalReader> signalReader_;


};


}; //namespace Picto

#endif
