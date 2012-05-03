#ifndef _LFP_DATA_ITERATOR_H_
#define _LFP_DATA_ITERATOR_H_
#include <QScriptEngine>
#include <QString>
#include <QLinkedList>
#include <QSqlDatabase>
#include <QHash>
#include "EventOrderIndex.h"
#include "AnalysisValue.h"

namespace Picto {
struct LFPData;
class LFPDataIterator
{
public:
	LFPDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session);
	virtual ~LFPDataIterator();
	bool isValid(){return true;};

	//Gets the next property change following the last one returned.  
	//If there are no more property changes available it returns a PropData struct
	//with negative time.
	QSharedPointer<LFPData> getNextLFPVals();
	unsigned int totalValues(){return readQueries_?totalQueries_ * (readValues_/readQueries_):totalQueries_;};
	unsigned int remainingValues(){return totalValues() - readValues_ + lfpVals_.size();};
	void recheckSessionData(){updateTotalQueryCount();};

private:
	void updateLFPValsList();
	void getSamplePeriod();
	void getAlignCoefficients();
	void updateTotalQueryCount();
	bool needMoreData();

	qulonglong lastSessionDataId_;
	struct lfpDBData
	{
		lfpDBData(double t,double st,unsigned int ch,QByteArray d){time=t;sampleTime=st;channel=ch;data=d;};
		double time;
		double sampleTime;
		unsigned int channel;
		QByteArray data;
	};
	
	QLinkedList<QSharedPointer<LFPData>> lfpVals_;
	struct TimestampRecord
	{
		TimestampRecord(){last = penult = -1;};
		double last;
		double penult;
	};
	QHash<int,TimestampRecord> timeRecs_;
	QHash<int,QLinkedList<QSharedPointer<LFPData>>::iterator> latestChEntries_;
	QSqlDatabase session_;
	QString tableName_;
	double samplePeriod_;
	double offsetTime_;
	double temporalFactor_;
	double notReadyYetTime_;
	bool sessionEnded_;
	unsigned int totalQueries_;
	unsigned int readValues_;
	unsigned int readQueries_;
	QSharedPointer<QScriptEngine> qsEngine_;
};

struct LFPData : public AnalysisValue{
	LFPData(QSharedPointer<QScriptEngine> qsEngine):AnalysisValue(qsEngine){value=0;channel=-1;}
	LFPData(QSharedPointer<QScriptEngine> qsEngine,double time,float val,unsigned int chan):AnalysisValue(qsEngine,EventOrderIndex(time)){/*index.time_ = time;*/value = val;channel = chan;scriptVal.setProperty("time",time);scriptVal.setProperty("value",value);scriptVal.setProperty("channel",channel);};
	float value;
	unsigned int channel;
};

}; //namespace Picto
#endif