#ifndef _ANALYSIS_DATA_ITERATOR_H_
#define _ANALYSIS_DATA_ITERATOR_H_
#include <QScriptEngine>
#include <QString>
#include <QLinkedList>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QObject>
#include "EventOrderIndex.h"
#include "AnalysisValue.h"

namespace Picto {
	class AnalysisDataIterator : public QObject
{
	Q_OBJECT
public:
	AnalysisDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session);
	virtual ~AnalysisDataIterator();

	//Gets the next property change following the last one returned.  
	//If there are no more property changes available it returns a PropData struct
	//with negative time.
	QSharedPointer<AnalysisValue> getNextValue();
	void setDataWindow(EventOrderIndex startFrom,EventOrderIndex endBefore);
	float fractionRemaining();
	void sessionDatabaseUpdated(){updateVariableSessionConstants();};
	virtual QString propertyDescriptor(){return "";};
	virtual EventOrderIndex::IDSource getDataSource() = 0;

protected:
	virtual void updateVariableSessionConstants(){};
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows) = 0;
	virtual bool prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime) = 0;
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query) = 0;
	//readOutRecordData must return the latest dataid.
	virtual qulonglong readOutRecordData(QSqlRecord* record) = 0;
	virtual unsigned int approxValsPerRow(){return 1;};
	virtual void dataFinishedWithSessionEnded(){};
	QSharedPointer<AnalysisValue> createNextAnalysisValue(EventOrderIndex index);
	QSqlQuery getSessionQuery();
	QScriptValue createScriptArray(unsigned int length=0);

private:
	void extractMainQueryResult(QSqlQuery* query);
	void updateAnalysisValsList();
	void updateTotalQueryCount();

	qulonglong lastSessionDataId_;

	QLinkedList<QSharedPointer<AnalysisValue>> analysisVals_;
	QSqlDatabase session_;
	EventOrderIndex startFrom_;
	EventOrderIndex endBefore_;
	unsigned int totalQueries_;
	unsigned int readQueries_;
	unsigned int totalValsCreated_;
	double firstValTime_;
	double lastValTime_;
	QSharedPointer<QScriptEngine> qsEngine_;
};

}; //namespace Picto
#endif