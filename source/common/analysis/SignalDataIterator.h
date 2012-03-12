#ifndef _SIGNAL_DATA_ITERATOR_H_
#define _SIGNAL_DATA_ITERATOR_H_
#include <QString>
#include <QLinkedList>
#include <QSqlDatabase>
#include "EventOrderIndex.h"

namespace Picto {
struct SignalData;
class SignalDataIterator
{
public:
	SignalDataIterator(QSqlDatabase session,QString signalName,QString subChannelName);
	virtual ~SignalDataIterator();
	bool isValid(){return subChanIndex_ >= 0;};

	//Gets the next property change following the last one returned.  
	//If there are no more property changes available it returns a PropData struct
	//with negative time.
	SignalData getNextSignalVals();
	unsigned int totalValues(){return totalValues_;};
	unsigned int remainingValues(){return signalVals_.size();};

private:
	void updateSignalValsList();
	void getSubChanInfo(QString signalName, QString subChanName);

	qulonglong lastSessionDataId_;
	
	QLinkedList<SignalData> signalVals_;
	QSqlDatabase session_;
	QString tableName_;
	int subChanIndex_;
	int numSubChans_;
	double samplePeriod_;
	unsigned int totalValues_;
};

struct SignalData{
	SignalData(){value="";}
	SignalData(qulonglong dataIndex,double time,float val){index.dataId_ = dataIndex;index.time_ = time;value = QString::number(val);};
	QString value;
	EventOrderIndex index;
};

}; //namespace Picto
#endif