#ifndef _LFP_DATA_ITERATOR_H_
#define _LFP_DATA_ITERATOR_H_
#include <QMultiMap>
#include "AnalysisDataIterator.h"

namespace Picto {
	class LFPDataIterator : public AnalysisDataIterator
{
public:
	LFPDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session);
	virtual ~LFPDataIterator();

protected:
	virtual void updateVariableSessionConstants();
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId);
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query);
	virtual qulonglong readOutRecordData(QSqlRecord* record);
	virtual void dataFinishedWithSessionEnded();

private:
	void outputEverythingBefore(double beforeTime);
	void addToOutputData(QSqlRecord record);
	void sortOutputDataAndCreateAnalysisValues();
	void getSamplePeriod();

	struct lfpData
	{
		lfpData(){time=-1;channel=-1;value=0;};
		lfpData(double t,unsigned int ch,float val){time=t;channel=ch;value=val;};
		inline bool operator<(const lfpData &other) const 
		{
			return time<other.time;
		}
		double time;
		unsigned int channel;
		float value;
	};
	QVector<lfpData> sortedData_;

	double samplePeriod_;
	double offsetTime_;
	double temporalFactor_;
	QMap<double,QMap<unsigned int,QSqlRecord>> timeSortedRecords_;
	QList<double> timesByChannel_;
};

}; //namespace Picto
#endif