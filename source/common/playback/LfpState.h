#ifndef _LfpState_H_
#define _LfpState_H_
#include <QVector>
#include <QStringList>
#include <QHash>
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Transition PlaybackData values for use in Playback system.
 */
class LfpState : public LfpReader, public DataState
{
	Q_OBJECT
public:
	LfpState();

	virtual void setDatabase(QSqlDatabase session);
	virtual void startRun(double runStartTime,double runEndTime = -1);
	virtual PlaybackIndex getCurrentIndex();
	virtual PlaybackIndex getNextIndex(double lookForwardTime);
	virtual void moveToIndex(PlaybackIndex index);

	//Lfp Reader Interface
	//Returns the available lfp channels
	virtual QVariantList getChannels();
	virtual double getSamplePeriod();
	virtual double getLatestTime();
	virtual double getLatestValue(int channel);
	virtual double getNextTime();
	virtual double getNextValue(int channel);
	//Returns signal values for the input lfp channel with times > the input time.
	virtual QVariantList getValuesSince(int channel,double time);
	virtual QVariantList getValuesUntil(int channel,double time);
	virtual QVariantList getTimesSince(double time);
	virtual QVariantList getTimesUntil(double time);

signals:
	void lfpChanged(int channel, double value);

private:
	PlaybackIndex getNextIndex();
	PlaybackIndex globalToRunIndex(PlaybackIndex index);
	QSqlDatabase session_;
	QSharedPointer<QSqlQuery> query_;
	double runStart_;
	double runEnd_;
	int curr_;
	struct PlaybackLFPData
	{
		//data_ = data;dataStart_ = reinterpret_cast<const float*>(data_.constData());dataSize_ = data_.size()/sizeof(float);
		PlaybackLFPData();
		~PlaybackLFPData();
		PlaybackLFPData(double time);
		void initializeValArray(int size);
		void setValue(int index,float value);
		inline bool operator<(const PlaybackLFPData& someData) const {
			return time_ < someData.time_;
		}
		float getValue(const int& index);
		double time_;
		float* channelVals_;
	};
	QList<PlaybackLFPData> data_;
	QHash<int,int> chanIndexMap_;
	QVariantList channels_;
	double sampPeriod_;
};

}; //namespace Picto
#endif