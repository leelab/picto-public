#ifndef _PROPERTY_DATA_ITERATOR_H_
#define _PROPERTY_DATA_ITERATOR_H_
#include <QString>
#include <QLinkedList>
#include <QSqlDatabase>
#include "EventOrderIndex.h"
#include "AnalysisValue.h"

namespace Picto {
struct PropData;
class PropertyDataIterator
{
public:
	PropertyDataIterator(QSqlDatabase session,QString propertyPath);
	virtual ~PropertyDataIterator();
	bool isValid(){return propertyId_ > 0;};

	//Gets the next property change following the last one returned.  
	//If there are no more property changes available it returns a PropData struct
	//with negative time.
	QSharedPointer<PropData> getNextPropertyChange();
	unsigned int totalValues(){return totalQueries_;};
	unsigned int remainingValues(){return totalValues() - readQueries_ + propVals_.size();};
	void recheckSessionData(){updateTotalQueryCount();};

private:
	void updatePropValsList();
	void updateTotalQueryCount();
	int getElementId(QString path);
	int getPropertyId(QString fullPath);
	static QString getParentElementPath(QString fullPath);
	static QString getPropertyName(QString fullPath);

	int propertyId_;
	qulonglong lastSessionDataId_;

	QLinkedList<QSharedPointer<PropData>> propVals_;
	QSqlDatabase session_;
	unsigned int totalQueries_;
	unsigned int readQueries_;

};

struct PropData : public AnalysisValue {
	PropData(){value="";}
	PropData(QString val,qulonglong dataIndex,double frameTime){value = val;index.dataId_ = dataIndex;index.time_ = frameTime; index.idSource_ = EventOrderIndex::BEHAVIORAL;};
	QString value;
};

}; //namespace Picto
#endif