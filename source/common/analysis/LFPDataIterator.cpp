#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include <QByteArray>
#include "../storage/datasourceinfo.h"
#include "../storage/alignmentinfo.h"
#include "LFPDataIterator.h"
using namespace Picto;

LFPDataIterator::LFPDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session)
{
	qsEngine_ = qsEngine;
	session_ = session;
	Q_ASSERT(session_.isValid() && session.isOpen());
	lastSessionDataId_ = 0;
	totalQueries_ = 0;
	readValues_ = 0;
	readQueries_ = 0;
	notReadyYetTime_ = 0;
	sessionEnded_ = false;

	getSamplePeriod();
	updateTotalQueryCount();
}

LFPDataIterator::~LFPDataIterator()
{

}

QSharedPointer<LFPData> LFPDataIterator::getNextLFPVals()
{
	if(!needMoreData())
	{
		return lfpVals_.takeFirst();
	}
	//Try getting new data.
	updateLFPValsList();
	if(!needMoreData())
	{
		return lfpVals_.takeFirst();
	}
	//No new data, return an empty propData()
	return QSharedPointer<LFPData>(new LFPData(qsEngine_));
}

void LFPDataIterator::updateLFPValsList()
{
	if(!isValid())
		return;
	if(readQueries_ >= totalQueries_)
		return;

	if(!sessionEnded_)
		getAlignCoefficients();

	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	//Check if session has ended
	query.prepare("SELECT key "
		"FROM sessioninfo WHERE key='sessionend'");
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}
	sessionEnded_ = query.next();

	//Get lfp value list.
	QString queryString = QString("SELECT dataid,timestamp,data,channel "
		"FROM lfp WHERE dataid > :lastDataId ORDER BY dataid LIMIT 1000");
	query.prepare(queryString);
	query.bindValue(":lastdataid",lastSessionDataId_);
	success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}

	QLinkedList<QSharedPointer<LFPData>>::iterator iter;
	double startTime;
	double currTime;
	double increment;
	unsigned int chan;
	while(query.next()){

		startTime = offsetTime_+temporalFactor_*query.value(1).toDouble();
		increment = temporalFactor_*samplePeriod_;
		chan = query.value(3).toInt();
		if(increment <= 0)
		{
			//The fittedsampleperiod is invalid.  Return.
			return;
		}
		
		//If this is a new channel move back in the array until we find the entry after where this data should be
		//added.
		if(!latestChEntries_.contains(chan))
		{
			iter = lfpVals_.end()-1;
			while(	(iter != lfpVals_.begin()) 
			&&(*iter)->index > currTime)
			{
				iter--;
			}
			latestChEntries_[chan] = iter;
		}
		//Set the iterator to the position where data was last entered by this channel.
		iter = latestChEntries_[chan];
		
		//IMPORTANT NOTE:
		//There are two issues that we need to deal with when returning lfp data to the analysis
		//system.  First of all, it could always occur that we will be asked for data up to a certain
		//time when one lfp channel has that time available, and another channel will have that time 
		//available, but it hasn't been written to the session yet (if this is running on a live experiment).
		//The second issue that we need to deal with is the fact that lfp data on a given channel could stop
		//coming in at a certain point if a user makes a change.
		//For this reason, we always store the last to lfp timestamps for each channel.  When we're done
		//sorting the incoming data, we look at the lowest value of the latest timestamp for each sample, 
		//we then compare this timestamp with the timestamps for each channel's second to last lfp
		//package.  If any of these timestamps are greater than the "lowest of the latest" timestamp, the
		//implication is that a channel's data stopped being recorded because if it hadn't it would be 
		//a full package late.  We keep doing this until we find a "lowest of the latest" timestamp that
		//is after all of the "second to last" timestamps, and we consider all data with timestamp after
		//that point to be "not yet ready."
		timeRecs_[chan].penult = timeRecs_[chan].last;
		timeRecs_[chan].last = startTime;

		//Convert data from blob to float array.  Get the relevant subchannel
		//and calculate its time.
		QByteArray dataByteArray = query.value(2).toByteArray();
		int numEntries = dataByteArray.size()/sizeof(float);
		float* floatArray = reinterpret_cast<float*>(dataByteArray.data());
		for(int i=0;i<numEntries;i++)
		{
			currTime = startTime + i*increment;
			while(	(iter != lfpVals_.end())
					&& ((*iter)->index <= currTime) )
				iter++;
			iter = lfpVals_.insert(iter,QSharedPointer<LFPData>(new LFPData(qsEngine_,currTime,floatArray[i],chan)));
			lastSessionDataId_ = query.value(0).toLongLong();
			readValues_++;
		}
		latestChEntries_[chan] = iter;	//Record the point of this channels latest entry

		readQueries_++;
	}

	//Find the first "notReadyYet" time.
	int minCh = -1;
	double minLast = -1;
	do {
		//Get minimum "last" time.
		minCh = -1;
		minLast = -1;
		for(QHash<int,TimestampRecord>::iterator iter = timeRecs_.begin();
			iter != timeRecs_.end();
			iter++)
		{
			if(minCh == -1)
			{
				minCh = iter.key();
				minLast = iter->last;
			}
			if(iter->last < minLast)
			{
				minCh = iter.key();
				minLast = iter->last;
			}
		}
		//Check for a penultimate time that's greater than min last time.
		for(QHash<int,TimestampRecord>::iterator iter = timeRecs_.begin();
		iter != timeRecs_.end();
		iter++)
		{
			if(iter->penult > minLast)
			{
				timeRecs_.remove(minCh);
				break;
			}
		}
	}while(!timeRecs_.contains(minCh));	//Keep looping until no penultimate time is greater than the min last time.  
										//Then the min last time is our notReadyYetTime_;
	notReadyYetTime_ = minLast;

	if(readQueries_ > totalQueries_)
		updateTotalQueryCount();
}

void LFPDataIterator::getSamplePeriod()
{
	samplePeriod_ = 0;
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	QString queryString = QString("SELECT value FROM sessioninfo WHERE key=\"DataSource\"");
	query.prepare(queryString);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}
	while(query.next())
	{
		DataSourceInfo src;
		src.fromXml(query.value(0).toString());
		if(src.getName() == "lfp")
		{
			samplePeriod_ = src.getResolution();
			break;
		}
	}
}

void LFPDataIterator::getAlignCoefficients()
{
	offsetTime_ = 0;
	temporalFactor_ = 0;
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	QString queryString = QString("SELECT value FROM sessioninfo WHERE key=\"AlignmentInfo\"");
	query.prepare(queryString);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}
	if(query.next())
	{
		AlignmentInfo inf;
		inf.fromXml(query.value(0).toString());
		offsetTime_ = inf.getOffsetTime();
		temporalFactor_ = inf.getTemporalFactor();
	}
}

void LFPDataIterator::updateTotalQueryCount()
{
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	QString queryString = QString("SELECT COUNT(dataid) FROM lfp");
	query.prepare(queryString);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}

	if(query.next())
		totalQueries_ = query.value(0).toInt();
}

bool LFPDataIterator::needMoreData()
{
	if(!lfpVals_.size())
		return true;
	if(sessionEnded_&&(readQueries_ >= totalQueries_))
		return false;
	if(lfpVals_.first()->index >= notReadyYetTime_)
		return true;
	return false;
}