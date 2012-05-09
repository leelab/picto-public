#ifndef _SIGNAL_DATA_ITERATOR_H_
#define _SIGNAL_DATA_ITERATOR_H_
#include "AnalysisDataIterator.h"

namespace Picto {
class SignalDataIterator : public AnalysisDataIterator
{
	Q_OBJECT
public:
	SignalDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session,QString signalName);
	virtual ~SignalDataIterator();

	unsigned int numSubChannels(){return numSubChans_;};
	QString subChanName(int subChanIndex){if(subChanNames_.size() < subChanIndex) return ""; return subChanNames_[subChanIndex];};
	virtual QString propertyDescriptor();
	virtual EventOrderIndex::IDSource getDataSource(){return EventOrderIndex::BEHAVIORAL;};

protected:
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId,double stopTime,unsigned int maxRows);
	virtual bool prepareSqlQueryForLastRowBeforeStart(QSqlQuery* query,double beforeTime);
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query);
	virtual qulonglong readOutRecordData(QSqlRecord* record);
	virtual unsigned int approxValsPerRow(){return approxValsPerRow_;};

private:
	bool isValid(){return numSubChans_ > 0;};
	void getSubChanInfo(QString signalName);

	QString tableName_;
	int numSubChans_;
	QStringList subChanNames_;
	double samplePeriod_;
	unsigned int approxValsPerRow_;
};

}; //namespace Picto
#endif