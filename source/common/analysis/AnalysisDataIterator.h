#ifndef _ANALYSIS_DATA_ITERATOR_H_
#define _ANALYSIS_DATA_ITERATOR_H_
#include <QScriptEngine>
#include <QString>
#include <QLinkedList>
#include <QSqlDatabase>
#include <QSqlRecord>
#include "EventOrderIndex.h"
#include "AnalysisValue.h"

namespace Picto {
class AnalysisDataIterator
{
public:
	AnalysisDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session);
	virtual ~AnalysisDataIterator();

	//Gets the next property change following the last one returned.  
	//If there are no more property changes available it returns a PropData struct
	//with negative time.
	QSharedPointer<AnalysisValue> getNextValue();
	float fractionRemaining();
	void recheckSessionData(){updateTotalQueryCount();updateVariableSessionConstants();};

protected:
	virtual void updateVariableSessionConstants(){};
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId) = 0;
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query) = 0;
	//readOutRecordData must return the latest dataid.
	virtual qulonglong readOutRecordData(QSqlRecord* record) = 0;
	virtual void dataFinishedWithSessionEnded(){};
	QSharedPointer<AnalysisValue> createNextAnalysisValue(EventOrderIndex index);
	QSqlQuery getSessionQuery();
	QScriptValue createScriptArray(unsigned int length=0);

private:
	void updateAnalysisValsList();
	void updateTotalQueryCount();

	qulonglong lastSessionDataId_;
	bool initialized_;
	int avgValsPerRow_;

	QLinkedList<QSharedPointer<AnalysisValue>> analysisVals_;
	QSqlDatabase session_;
	unsigned int totalQueries_;
	unsigned int readQueries_;
	unsigned int totalValsCreated_;
	QSharedPointer<QScriptEngine> qsEngine_;
};

}; //namespace Picto
#endif