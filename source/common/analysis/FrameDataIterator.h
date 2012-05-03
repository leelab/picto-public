#ifndef _FRAME_DATA_ITERATOR_H_
#define _FRAME_DATA_ITERATOR_H_
#include <QScriptEngine>
#include <QString>
#include <QLinkedList>
#include <QSqlDatabase>
#include "EventOrderIndex.h"
#include "AnalysisValue.h"

namespace Picto {
struct FrameData;
class FrameDataIterator
{
public:
	FrameDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session);
	virtual ~FrameDataIterator();

	//Gets the next property change following the last one returned.  
	//If there are no more property changes available it returns a PropData struct
	//with negative time.
	QSharedPointer<FrameData> getNextFrameChange();
	unsigned int totalValues(){return totalQueries_;};
	unsigned int remainingValues(){return totalValues() - readQueries_ + frameVals_.size();};
	void recheckSessionData(){updateTotalQueryCount();};

private:
	void updateFrameValsList();
	void updateTotalQueryCount();

	qulonglong lastSessionDataId_;

	QLinkedList<QSharedPointer<FrameData>> frameVals_;
	QSqlDatabase session_;
	unsigned int totalQueries_;
	unsigned int readQueries_;
	QSharedPointer<QScriptEngine> qsEngine_;
};

struct FrameData : public AnalysisValue {
	//FrameData(){}
	FrameData(QSharedPointer<QScriptEngine> qsEngine):AnalysisValue(qsEngine){}
	FrameData(QSharedPointer<QScriptEngine> qsEngine,qulonglong dataIndex,double frameTime):AnalysisValue(qsEngine,EventOrderIndex(frameTime,dataIndex,EventOrderIndex::BEHAVIORAL)){/*index.dataId_ = dataIndex;index.time_ = frameTime; index.idSource_ = EventOrderIndex::BEHAVIORAL;*/scriptVal.setProperty("time",frameTime);};
};

}; //namespace Picto
#endif