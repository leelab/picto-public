#ifndef _TRANSITION_DATA_ITERATOR_H_
#define _TRANSITION_DATA_ITERATOR_H_
#include "AnalysisDataIterator.h"

namespace Picto {
	class TransitionDataIterator : public AnalysisDataIterator
{
public:
	TransitionDataIterator(QSharedPointer<QScriptEngine> qsEngine,QSqlDatabase session);
	virtual ~TransitionDataIterator();
	void registerTransitions(QString parentPath,QString sourceName = "",QString resultName = "", QString destinationName = "");
	void registerTransitionsByNode(QString nodePath,bool isSource);
	void registerTransitionsByResult(QString resultPath);
	void registerTransitionsBySource(QString sourcePath);

protected:
	virtual bool prepareSqlQuery(QSqlQuery* query,qulonglong lastDataId);
	virtual void prepareSqlQueryForTotalRowCount(QSqlQuery* query);
	virtual qulonglong readOutRecordData(QSqlRecord* record);

private:
	bool isValid(){return !transIdString_.isEmpty() && valid_;};
	int getElementId(QString path);
	QString transIdString_;
	bool valid_;
};

}; //namespace Picto
#endif