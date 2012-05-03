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
	ElementDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session);
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
	QSharedPointer<QScriptEngine> qsEngine_;
};

struct ElementData : public AnalysisValue {
	//ElementData(){}
	ElementData(QSharedPointer<QScriptEngine> qsEngine):AnalysisValue(qsEngine){}
	ElementData(QSharedPointer<QScriptEngine> qsEngine,qulonglong dataIndex,double time,QString path):AnalysisValue(qsEngine,EventOrderIndex(time,dataIndex,EventOrderIndex::BEHAVIORAL)){/*index.dataId_ = dataIndex;index.time_ = time; index.idSource_ = EventOrderIndex::BEHAVIORAL;*/path_ = path;scriptVal.setProperty("time",time);scriptVal.setProperty("name",path);};
	QString path_;
};

}; //namespace Picto
#endif