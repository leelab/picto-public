#ifndef _LiveLfpReader_H_
#define _LiveLfpReader_H_
#include <QVector>
#include <QHash>
#include "../../common/playback/PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Signal PlaybackData values for use in Playback system.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API LiveLfpReader : public LfpReader
#else
class LiveLfpReader : public LfpReader
#endif
{
	Q_OBJECT
public:
	LiveLfpReader(QList<int> channels,double samplePeriod);
	void setRunStart();
	void createVirtualLfpData(double time);
	void setRunEnd();

	//Lfp Reader Interface
	//Returns the available lfp channels
	virtual QVariantList getChannels();
	virtual double getSamplePeriod();
	virtual double getLatestTime();
	virtual double getLatestValue(int channel);
	virtual double getNextTime();
	virtual double getNextValue(QString channel);
	//Returns signal values for the input lfp channel with times > the input time.
	virtual QVariantList getValuesSince(int channel,double time);
	virtual QVariantList getValuesUntil(int channel,double time);
	virtual QVariantList getTimesSince(double time);
	virtual QVariantList getTimesUntil(double time);

private:
	QVector<double> periods_;
	QVector<double> amplitudes_;
	float getValue(int channel, double time);
	QVector<double> data_;
	QHash<int,int> chanIndexMap_;
	QVariantList channels_;
	int numChannels_;
	double samplePeriod_;
};

}; //namespace Picto
#endif