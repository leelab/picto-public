#ifndef _TRANSITION_DATA_ITERATOR_H_
#define _TRANSITION_DATA_ITERATOR_H_
#include <QString>
#include <QLinkedList>
#include <QSqlDatabase>
#include "EventOrderIndex.h"

namespace Picto {
class TransitionDataIterator
{
public:
	TransitionDataIterator(QSqlDatabase session);
	virtual ~TransitionDataIterator();
	void registerTransitions(QString parentPath,QString sourceName = "",QString resultName = "", QString destinationName = "");
	void registerTransitionsByNode(QString nodePath,bool isSource);
	void registerTransitionsByResult(QString resultPath);
	void registerTransitionsBySource(QString sourcePath);
	bool isValid(){return !transIdString_.isEmpty() && valid_;};

	//Gets the next transition traversal following the last one returned.  
	//If there are no more traversals available it returns an EventOrderIndex
	//with negative time.
	EventOrderIndex getNextTransitionTraversal();
	unsigned int totalValues(){return totalQueries_;};
	unsigned int remainingValues(){return totalValues() - readQueries_+traversals_.size();};
	void recheckSessionData(){updateTotalQueryCount();};

private:
	void updateTraversalList();
	void updateTotalQueryCount();
	int getElementId(QString path);

	QString transIdString_;
	qulonglong lastSessionDataId_;

	QLinkedList<EventOrderIndex> traversals_;
	QSqlDatabase session_;
	bool valid_;
	unsigned int totalQueries_;
	unsigned int readQueries_;
};

}; //namespace Picto
#endif