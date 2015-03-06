#ifndef _LiveSignalReader_H_
#define _LiveSignalReader_H_
#include <QVector>
#include <QHash>
#include "../../common/playback/PlaybackInterfaces.h"

namespace Picto {
/*! \brief Implements the SignalReader class for a live test experiment.
 *	\details This object is filled during run time by using the setLatestSignalData() function such that Analyses 
 *	have access to all past data but no future data.  Requests for future data from this class will result in 
 *	meaningless values.
 *	\note Since the functions here simply implement the SignalReader class for
 *	data read in during a live Session, there is not much to add in terms of documentation 
 *	beyond what was described above, so we will not be adding additional function level documentation
 *	for many of the functions in this class.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API LiveSignalReader : public SignalReader
#else
class LiveSignalReader : public SignalReader
#endif
{
	Q_OBJECT
public:
	LiveSignalReader(QString name,QStringList subChanNames,double samplePeriod);
	void setRunStart();
	void setLatestSignalData(double time,QVector<float> vals);
	void setRunEnd();

	//Signal Reader Interface
	virtual QString getName();
	virtual QStringList getComponentNames();
	virtual double getSamplePeriod();
	virtual double getLatestTime();
	virtual double getLatestValue(QString channel);
	virtual double getNextTime();
	virtual double getNextValue(QString channel);
	virtual QVariantList getValuesSince(QString channel,double time);
	virtual QVariantList getValuesUntil(QString channel,double time);
	virtual QVariantList getTimesSince(double time);
	virtual QVariantList getTimesUntil(double time);

private:
	struct PlaybackSignalData
	{
		PlaybackSignalData(){};
		PlaybackSignalData(double time){time_= time;};
		PlaybackSignalData(double time,QVector<float> vals){time_= time;vals_ = vals;};
		inline bool operator<(const PlaybackSignalData& someData) const {
			return time_ < someData.time_;
		}
		double time_;
		QVector<float> vals_;
	};
	QVector<PlaybackSignalData> signalData_;
	QString name_;
	QStringList subChanNames_;
	int numSubChans_;
	QHash<QString,int> subChanIndexLookup_;
	double samplePeriod_;
};

}; //namespace Picto
#endif