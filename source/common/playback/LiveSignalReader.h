#ifndef _LiveSignalReader_H_
#define _LiveSignalReader_H_
#include <QVector>
#include <QHash>
#include "../../common/playback/PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Signal PlaybackData values for use in Playback system.
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
	//Returns the name of the signal handled by this signal reader
	virtual QString getName();
	//Returns the sub channels of this signal
	virtual QStringList getComponentNames();
	virtual double getSamplePeriod();
	virtual double getLatestTime();
	virtual double getLatestValue(QString channel);
	virtual double getNextTime();
	virtual double getNextValue(QString channel);
	//Returns signal values for the input sub channel with times > the input time.
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