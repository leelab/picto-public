#ifndef _FRAME_DATA_ITERATOR_H_
#define _FRAME_DATA_ITERATOR_H_
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
	FrameDataIterator(QSqlDatabase session);
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

};

struct FrameData : public AnalysisValue {
	FrameData(){}
	FrameData(qulonglong dataIndex,double frameTime){index.dataId_ = dataIndex;index.time_ = frameTime; index.idSource_ = EventOrderIndex::BEHAVIORAL;};
};

}; //namespace Picto
#endif