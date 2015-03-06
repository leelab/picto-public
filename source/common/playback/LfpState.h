#ifndef _LfpState_H_
#define _LfpState_H_
#include <QVector>
#include <QStringList>
#include <QHash>
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
/*! \brief Implements the DataState and LfpReader classes to load a Picto Session database, 
 *	extract the lfp data and implement functions for traversing through that data.
 *	\details The class is fairly simple, a QList of QLists of Local Field Potential data is loaded from the 
 *	session data where each sub QList represents a single LFP channel.  All sub QLists are the same length
 *	with each index representing a single time, offset by getSamplePeriod() from the prior value and starting
 *	from the first lfp data point in the session file.  When moveToIndex() is called, we just traverse through the list until we reach 
 *	the appropriate time.  Each time moveToIndex() causes us to pass through an LFP read time, the lfpChanged() 
 *	signal is called for each channel, which tells the rest of the playback system that lfp values changed.
 *
 *	In many cases not all channels start recording at the same time.  In these cases the QList values
 *	for those channels are stored as zero until their real data starts coming in.  This can clearly be
 *	optimized at some point by storing a list of start times to save RAM.
 *
 *	\note Since the functions here simply implement the LfpReader and DataState classes for
 *	data read in from a Session Database, there is not much to add in terms of documentation 
 *	beyond what was described above, so we will not be adding additional function level documentation
 *	for many of the functions in this class.
 *
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	/*! \brief Emitted when moveToIndex() moves the current time passed a new lfp value.  channel is the
	 *	channel on which the value occured, value is the value.
	 */
	void lfpChanged(int channel, double value);
	/*! \brief Emitted to signify the percentage of LFP data that has been loaded so far in the 
	 *	loadData() function.  Values are from 0 to 100.
	 */
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