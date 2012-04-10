#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "FrameDataIterator.h"
using namespace Picto;

FrameDataIterator::FrameDataIterator(QSqlDatabase session)
{
	session_ = session;
	Q_ASSERT(session_.isValid() && session.isOpen());
	lastSessionDataId_ = 0;
	totalQueries_ = 0;
	readQueries_ = 0;

	updateTotalQueryCount();
}

FrameDataIterator::~FrameDataIterator()
{

}

QSharedPointer<FrameData> FrameDataIterator::getNextFrameChange()
{
	if(frameVals_.size())
		return frameVals_.takeFirst();
	//Maybe the session wasn't over last time.  Try getting new data.
	updateFrameValsList();
	if(frameVals_.size())
		return frameVals_.takeFirst();
	//No new data, return an empty frameData()
	return QSharedPointer<FrameData>(new FrameData());
}

void FrameDataIterator::updateFrameValsList()
{
	if(readQueries_ >= totalQueries_)
		return;
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	//Get frame value list.
	query.prepare("SELECT f.dataid, f.time FROM frames f WHERE f.dataid > :lastdataid ORDER BY f.dataid LIMIT 10000");
	query.bindValue(":lastdataid",lastSessionDataId_);
	bool success = query.exec();
	if(!success)
	{
		return;
	}
	qulonglong lastDataId = lastSessionDataId_;
	while(query.next()){
		frameVals_.append(QSharedPointer<FrameData>(new FrameData(query.value(0).toLongLong(),query.value(1).toDouble())));
		lastDataId = query.value(0).toLongLong();
		readQueries_++;
	}
	if(readQueries_ > totalQueries_)
		updateTotalQueryCount();

	lastSessionDataId_ = lastDataId;
}

void FrameDataIterator::updateTotalQueryCount()
{
	Q_ASSERT(session_.isValid() && session_.isOpen());
	QSqlQuery query(session_);
	query.setForwardOnly(true);

	QString queryString = QString("SELECT COUNT(dataid) FROM frames");
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