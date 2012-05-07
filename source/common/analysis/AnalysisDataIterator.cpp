#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include <QCoreApplication>
#include "AnalysisDataIterator.h"
using namespace Picto;

AnalysisDataIterator::AnalysisDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session)
{
	qsEngine_ = qsEngine;
	session_ = session;
	Q_ASSERT(session_.isValid() && session.isOpen());
	lastSessionDataId_ = 0;
	totalQueries_ = 0;
	readQueries_ = 0;
	avgValsPerRow_ = 0;
	totalValsCreated_ = 0;
	initialized_ = false;
}

AnalysisDataIterator::~AnalysisDataIterator()
{

}

QSharedPointer<AnalysisValue> AnalysisDataIterator::getNextValue()
{
	if(!initialized_)
	{
		recheckSessionData();
		initialized_ = true;
	}
	QCoreApplication::processEvents();
	if(analysisVals_.size())
		return analysisVals_.takeFirst();
	//Maybe the session wasn't over last time.  Try getting new data.
	updateAnalysisValsList();
	if(analysisVals_.size())
		return analysisVals_.takeFirst();
	//No new data, return an empty AnalysisValue()
	return QSharedPointer<AnalysisValue>(new AnalysisValue(qsEngine_));
}

float AnalysisDataIterator::fractionRemaining()
{
	if(!totalQueries_ || !totalValsCreated_)
		return 1.0;
	float fracQueriesRead = float(readQueries_)/float(totalQueries_);
	float fracValsRead = float(totalValsCreated_-analysisVals_.size())/float(totalValsCreated_);
	return 1.0-(fracQueriesRead*fracValsRead);
}

QSharedPointer<AnalysisValue> AnalysisDataIterator::createNextAnalysisValue(EventOrderIndex index)
{
	totalValsCreated_++;
	QSharedPointer<AnalysisValue> returnVal(new AnalysisValue(qsEngine_,index));
	analysisVals_.append(returnVal);
	return returnVal;
}

QSqlQuery AnalysisDataIterator::getSessionQuery()
{
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	return query;
}

QScriptValue AnalysisDataIterator::createScriptArray(unsigned int length)
{
	return qsEngine_->newArray(length);
}

void AnalysisDataIterator::updateAnalysisValsList()
{
	if(readQueries_ >= totalQueries_)
		return;
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	//Get frame value list.
	if(!prepareSqlQuery(&query,lastSessionDataId_))
		return;
	bool success = query.exec();
	if(!success)
	{
		return;
	}
	qulonglong lastDataId = lastSessionDataId_;
	if(!query.next())
	{
		//There is no more data available.  Check if session has ended and if so
		//tell the child class to finish up with any data it may have been holding 
		//on too.
		QSqlQuery query = getSessionQuery();

		//Check if session has ended
		query.prepare("SELECT key "
			"FROM sessioninfo WHERE key='sessionend'");
		bool success = query.exec();
		if(!success)
		{
			qDebug("Query Failed: " + query.lastError().text().toAscii());
			return;
		}
		if(query.next())
		{	//The session has ended
			dataFinishedWithSessionEnded();
		}
		return;
	}
	do{
		readQueries_++;
		lastDataId = readOutRecordData(&query.record());
	}while(query.next());
	if(readQueries_ > totalQueries_)
		recheckSessionData();

	lastSessionDataId_ = lastDataId;
}

void AnalysisDataIterator::updateTotalQueryCount()
{
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	prepareSqlQueryForTotalRowCount(&query);
	bool success = query.exec();
	if(!success)
	{
		qDebug("Query Failed: " + query.lastError().text().toAscii());
		return;
	}

	if(query.next())
		totalQueries_ = query.value(0).toInt();
}