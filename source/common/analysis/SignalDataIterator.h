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

protected:
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId);
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query);
	virtual qulonglong readOutRecordData(QSqlRecord* record);

private:
	bool isValid(){return numSubChans_ > 0;};
	void getSubChanInfo(QString signalName);

	QString tableName_;
	int numSubChans_;
	QStringList subChanNames_;
	double samplePeriod_;
	unsigned int readValues_;
	unsigned int readQueries_;
};

}; //namespace Picto
#endif