#ifndef _LFPDATAUNITPACKAGE_H_
#define _LFPDATAUNITPACKAGE_H_

#include "../common.h"
#include "DataUnit.h"
#include <QString>
#include <QVector>
#include <QList>
#include <QMap>


namespace Picto {

struct lfpDataBlock
{
	lfpDataBlock(double timestamp,double timePerSample){timestamp_=timestamp;timePerSample_=timePerSample;};
	QMap<int,QList<double>> data;
	double timestamp_;
	double timePerSample_;
};

/*!	\brief Stores local field data
 *
 *	Currently, local field potential data storage is implemented as a one shot deal.
 *	At the end of a session, all local field data is sent to server as a single 
 *	data store.  It is meant to be written to the lfp database once
 *	with aligned timestamps, with one row per channel per session.
 *	The cleaner way to do this would be to create one row per timestamp
 *	with columns for each channel, but with the current database Sqlite
 *	this simply takes too long.  We ran a benchmark test and found that
 *	each character stored in a new row takes an additional ~20us, whereas
 *	each character stored in the same row takes an additional ~20ns.
 *	Big difference.
 *	A typical session can include millions of timestamps, which means
 *	that even the most efficient of Sql commands would require multiple
 *	minutes of processing power over the course of the session, even if 
 *  we could somehow write everything at once.  
 *	This is too much.  
 *	By putting everything on one line, we can bring this time down to 
 *	tens of milliseconds of processing, which is much more doable.  Possibly
 *	we can add post session background database conversion at some
 *	point to move the values into separate rows by timestamp.
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API LFPDataUnitPackage : public DataUnit
#else
class LFPDataUnitPackage : public DataUnit
#endif
{
public:
	LFPDataUnitPackage();
	//Data store functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void setCorrelation(double correlation){correlation_ = correlation;}
	void addData(double timestamp, double* potentials, int numVals);
	void addData(double timestamp, double* potentials, int numVals, double fittedtime);
	void addDataByBlock(lfpDataBlock* block);
	int numSamples(){return numSamples_;};

	double getCorrelation(){return correlation_;}
	QString getTimes(){return times_;};
	QString getFittedTimes(){return fittedTimes_;};
	QVector<QString> getPotentials(){return potentials_;};

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	int channel_;
	double correlation_;
	QString times_;
	QString fittedTimes_;
	QVector<QString> potentials_;
	int numSamples_;

};


}; //namespace Picto

#endif
