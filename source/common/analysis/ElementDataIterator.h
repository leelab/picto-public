#ifndef _ELEMENT_DATA_ITERATOR_H_
#define _ELEMENT_DATA_ITERATOR_H_
#include <QString>
#include <QLinkedList>
#include <QSqlDatabase>
#include "EventOrderIndex.h"
#include "AnalysisValue.h"

namespace Picto {
struct ElementData;
class ElementDataIterator
{
public:
	ElementDataIterator(QSqlDatabase session);
	virtual ~ElementDataIterator();

	//Gets the Element that ran next after the last one returned.  
	//If there are no more Elements available it returns an empty pointer.
	QSharedPointer<ElementData> getNextElement();
	unsigned int totalValues(){return totalQueries_;};
	unsigned int remainingValues(){return totalValues() - readQueries_+elements_.size();};
	void recheckSessionData(){updateTotalQueryCount();};

private:
	void updateElementList();
	void updateTotalQueryCount();

	qulonglong lastSessionDataId_;

	QLinkedList<QSharedPointer<ElementData>> elements_;
	QSqlDatabase session_;
	unsigned int totalQueries_;
	unsigned int readQueries_;
};

struct ElementData : public AnalysisValue {
	ElementData(){}
	ElementData(qulonglong dataIndex,double time,QString path){index.dataId_ = dataIndex;index.time_ = time; index.idSource_ = EventOrderIndex::BEHAVIORAL;path_ = path;};
	QString path_;
};

}; //namespace Picto
#endif