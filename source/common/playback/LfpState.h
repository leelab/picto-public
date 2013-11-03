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
	LfpState(bool enabled = true);

	void setEnabled(bool enable);
	bool getEnabled();
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
	void lfpLoadProgress(int percent);

private:
	bool loadData();
	void resetDataVariables();
	PlaybackIndex getNextIndex();
	PlaybackIndex globalToRunIndex(PlaybackIndex index);
	double globalTimeFromArrayIndex(const int& index);
	int arrayIndexFromGlobalTime(const double& time);
	float getDataValue(const int& arrayIndex,const int& channelIndex);
	void setDataValue(const int& arrayIndex,const int& channelIndex,const float& value);
	QSqlDatabase session_;
	bool enabled_;
	QSharedPointer<QSqlQuery> query_;
	double runStart_;
	double runEnd_;
	int curr_;
	QList<QList<float>> data_;
	QHash<int,int> chanIndexMap_;
	QVariantList channels_;
	int numChannels_;
	int numValues_;
	double sampPeriod_;
	double minLfpTime_;
};

}; //namespace Picto
#endif