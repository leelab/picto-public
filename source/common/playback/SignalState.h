#ifndef _SIGNALSTATE_H_
#define _SIGNALSTATE_H_
#include <QVector>
#include <QStringList>
#include <QHash>
#include "DataState.h"
#include "PlaybackInterfaces.h"

namespace Picto {
struct PlaybackSignalData;
/*! \brief Implements the DataState and SignalReader classes to load a Picto Session database, 
 *	extract Signal values for a particular channel and implement functions for traversing through that data.
 *	\details The class is fairly simple, a QList of PlaybackSignalData objects is loaded from the 
 *	session data.  Each PlaybackSignalData object represents a single sample time and when moveToIndex() 
 *	is called, we just traverse through the list until we reach a PlaybackSignalData with the appropriate time.
 *	Each time moveToIndex() causes us to pass through a PlaybackSignalData entry, the signalChanged() 
 *	(Qt) signal is emitted, which tells the rest of the playback system that new signal values have been read.
 *
 *	\note Since the functions here simply implement the SignalReader and DataState classes for
 *	data read in from a Session Database, there is not much to add in terms of documentation 
 *	beyond what was described above, so we will not be adding function level documentation
 *	for this class.
 *
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class SignalState : public SignalReader, public DataState
{
	Q_OBJECT
public:
	SignalState(QString name,QString tableName,QStringList subChanNames,double sampPeriod);

	virtual void setDatabase(QSqlDatabase session);
	virtual void startRun(double runStartTime,double runEndTime = -1);
	virtual PlaybackIndex getCurrentIndex();
	virtual PlaybackIndex getNextIndex(double lookForwardTime);
	virtual void moveToIndex(PlaybackIndex index);

	//Signal Reader Interface
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

signals:
	/*! \brief Emitted whenever a Signal data sample is traversed due to a call to moveToIndex().
	 *	\details name is the name of the Signal channel, subChanNames is a list of the names of
	 *	that signals sub channels, value is a Vector of the new float values of those signals, where
	 *	they are listed in the same order as subChanNames.  Values returned have already been calibrated
	 *	according to the experimental calibration parameters set in the remote viewer during the experiment.
	 */
	void signalChanged(QString name, QStringList subChanNames,QVector<float> vals);

private:
	void goToNext();
	PlaybackIndex getNextIndex();
	PlaybackIndex globalToRunIndex(PlaybackIndex index);
	bool moveIndecesToNextTime(int& outerIndex, int& innerIndex);
	QSqlDatabase session_;
	double runStart_;
	double runEnd_;
	int curr_;
	int currSub_;
	QList<PlaybackSignalData> data_;

	QString name_;
	QString tableName_;
	QStringList subChanNames_;
	QHash<QString,int> subChanIndexLookup_;
	int numSubChans_;
	double sampPeriod_;
};

/*! \brief A struct used to store a signal channel sample.
 *	\details Includes the time at which the signal sample occured and a QVector of values for that sample 
 *	ordered like getComponentNames().
 */
struct PlaybackSignalData
{
	PlaybackSignalData(){};
	PlaybackSignalData(double time){time_= time;};
	/*! \brief One PlaybackSignalData is lower than another if its time_ is lower.*/
	inline bool operator<(const PlaybackSignalData& someData) const {
		return time_ < someData.time_;
	}
	double time_;
	QVector<float> vals_;
};

}; //namespace Picto
#endif