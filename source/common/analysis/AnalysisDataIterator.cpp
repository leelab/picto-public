#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include <QCoreApplication>
#include "AnalysisDataIterator.h"
using namespace Picto;

#define MAX_ANALYSIS_VALS_SIZE 100000

AnalysisDataIterator::AnalysisDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session)
{
	qsEngine_ = qsEngine;
	session_ = session;
	Q_ASSERT(session_.isValid() && session.isOpen());
	lastSessionDataId_ = 0;
	totalQueries_ = 0;
	readQueries_ = 0;
	totalValsCreated_ = 0;
	firstValTime_ = lastValTime_ = -1;

}

AnalysisDataIterator::~AnalysisDataIterator()
{

}

QSharedPointer<AnalysisValue> AnalysisDataIterator::getNextValue()
{
	Q_ASSERT_X(startFrom_.isValid(),"AnalysisDataIterator::getNextValue",
				"Window start time must be set before requesting a value");
	QCoreApplication::processEvents();
	if(analysisVals_.size())
		return analysisVals_.takeFirst();
	//Try getting more data.
	updateAnalysisValsList();
	if(analysisVals_.size())
		return analysisVals_.takeFirst();
	//No new data, return an empty AnalysisValue()
	return QSharedPointer<AnalysisValue>(new AnalysisValue(qsEngine_));
}

void AnalysisDataIterator::setDataWindow(EventOrderIndex startFrom,EventOrderIndex endBefore)
{
	if((startFrom == startFrom_)&&(endBefore==endBefore_))
		return;
	Q_ASSERT(startFrom.isValid() && (!endBefore.isValid() || (endBefore>=startFrom)));
	startFrom_ = startFrom;
	endBefore_ = endBefore;
	analysisVals_.clear();
	totalValsCreated_ = 0;
	totalQueries_ = 0;
	sessionDatabaseUpdated();

	//Get one value from before this window so that the initial data state is available
	EventOrderIndex beginning = startFrom;
	beginning.dataId_ = 0;
	beginning.time_ = 0;

	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);
	if(!prepareSqlQueryForLastRowBeforeStart(&query,startFrom_.isValid()?startFrom_.time_:100000000000))
		return;
	if(!query.exec())
	{
		Q_ASSERT(false);
		return;
	}
	if(!query.next())
		return;
	do
	{
		extractMainQueryResult(&query);
	}while(query.next());
}

float AnalysisDataIterator::fractionRemaining()
{
	if(firstValTime_ == lastValTime_)
		return 1.0;
	double timeSoFar = lastValTime_ - firstValTime_;
	double valsPerSec = double(totalValsCreated_)/timeSoFar;
	int approxTotalVals = endBefore_.isValid()?(endBefore_.time_-startFrom_.time_)*valsPerSec:100000000000*valsPerSec;
	int remainingVals = approxTotalVals-totalValsCreated_+analysisVals_.size();
	float returnVal = float(remainingVals)/float(approxTotalVals);
	if(returnVal < 0)
		returnVal = 0;
	return returnVal;
}

QSharedPointer<AnalysisValue> AnalysisDataIterator::createNextAnalysisValue(EventOrderIndex index)
{
	totalValsCreated_++;
	if(firstValTime_ == -1)
		firstValTime_ = index.time_;
	lastValTime_ = index.time_;
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

void AnalysisDataIterator::extractMainQueryResult(QSqlQuery* query)
{
	readQueries_++;
	lastSessionDataId_ = readOutRecordData(&(query->record()));
}

void AnalysisDataIterator::updateAnalysisValsList()
{
	Q_ASSERT(session_.isValid() && session_.isOpen());
	Q_ASSERT(startFrom_.isValid() && (!endBefore_.isValid() || (endBefore_>=startFrom_)));
	QSqlQuery query(session_);
	query.setForwardOnly(true);
	bool res;
	int maxRows = 0;
	if(approxValsPerRow())
		maxRows = MAX_ANALYSIS_VALS_SIZE/approxValsPerRow();
	if(!prepareSqlQuery(&query,lastSessionDataId_,endBefore_.isValid()?endBefore_.time_:100000000000,maxRows))
		return;
	if(!query.exec())
	{
		Q_ASSERT(false);
	}
	if(!query.next())
	{
		//There is no more data available.  Check if session has ended and if so
		//tell the child class to finish up with any data it may have been holding 
		//on too.

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
	totalQueries_ = query.size()+readQueries_;
	do{
		extractMainQueryResult(&query);
	}while(query.next() && (analysisVals_.size() < MAX_ANALYSIS_VALS_SIZE));
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