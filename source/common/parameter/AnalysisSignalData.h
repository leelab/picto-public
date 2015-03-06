#ifndef _AnalysisSignalData_H_
#define _AnalysisSignalData_H_

#include <QWeakPointer>
#include "../common.h"

#include "AnalysisDataSource.h"

namespace Picto {

/*!	\brief An AnalysisDataSource for gathering signal data.
 *	\details This object can be used to query signal data for any input signal as well as
 *	metadata including the sub components of the signal and its sample period.
 *	The signal tracked is set in the SignalName property, where "Position" and "Diameter"
 *	are the standard signal names, but theoretically, there could be any number of signals 
 *	with different names.  They are defined in the Director's HardwareSetup by using 
 *	Engine::addSignalChannel().
 *
 *	\note This class uses a SignalReader as its main data source.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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

	virtual void enteredScope();

	virtual QString friendlyTypeName(){return "Signal Data";};
	virtual QString getUITemplate(){return "SignalDataSource";};
	virtual QString getUIGroup(){return "Sensors";};

public slots:
	void zeroLatestFrame();
	virtual QStringList getComponentNames();
	double getSamplePeriod();
	double getLatestTime();
	double getLatestValue(QString componentName);
	QVariantList getLatestValue();
	double getNextTime();
	double getNextValue(QString componentName);
	QVariantList getNextValue();
	QVariantList getPrevTimes(double secsPreceding);
	QVariantList getNextTimes(double secsFollowing);
	QVariantList getTimesSince(double beyondTime);
	QVariantList getTimesUntil(double upToTime);
	QVariantList getPrevValues(QString componentName,double secsPreceding);
	QVariantList getNextValues(QString componentName,double secsFollowing);
	QVariantList getPrevValues(double secsPreceding);
	QVariantList getNextValues(double secsFollowing);
	QVariantList getValuesSince(QString componentName,double beyondTime);
	QVariantList getValuesUntil(QString componentName,double upToTime);
	QVariantList getValuesSince(double beyondTime);
	QVariantList getValuesUntil(double upToTime);
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	double getLatestRunTime();
	double zeroTime_;
	QSharedPointer<SignalReader> signalReader_;


};


}; //namespace Picto

#endif
