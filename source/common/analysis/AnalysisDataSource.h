#ifndef _ANALYSIS_DATA_SOURCE_H_
#define _ANALYSIS_DATA_SOURCE_H_

#include <QVariant>
#include <QScriptValue>
#include <QSqlDatabase>
#include <QLinkedList>
#include "EventOrderIndex.h"
#include "../statemachine/UIEnabled.h"
#include "AnalysisValue.h"

namespace Picto {
#if defined WIN32 || defined WINCE
class PICTOLIB_API AnalysisDataSource : public UIEnabled
#else
class AnalysisDataSource : public UIEnabled
#endif
{
	Q_OBJECT
public:
	AnalysisDataSource();
	virtual ~AnalysisDataSource();

	void loadSession(QSqlDatabase session){session_ = session;};

	//AnalysisDataSource specific functions
	//Should be implemented by child classes to reset to an initial state.
	//In particular, getValue(time) will always have an increasing input
	//time until reset is called.
	virtual void restart() = 0;
	void sessionDatabaseUpdated();

	void clearValuesTo(const EventOrderIndex& index);
	void storeValue(const EventOrderIndex& index);
	//Tells the DataSource to prepare all values occuring before beforeIndex for scripting.
	//Internally, values are copied from a linkedlist to an array for fast indexing.
	void prepareValuesForScript(const EventOrderIndex& beforeIndex);
	unsigned int getNumScriptValues(){return usefulScriptValues_;};

	//Should be extended by child classes to return their value at the input time.  The input
	//value to this function will always increase until restart is called.  This means that 
	//child classes can store previous values and iterate forward to get the value at a new 
	//time rather than always restarting their search at zero.
	virtual QSharedPointer<AnalysisValue> getValue(const EventOrderIndex& index) = 0;

	//Inherited
	virtual QString getUITemplate(){return "DataSource";};
	virtual QString assetType(){return "DataSource";};

protected:

	virtual void recheckSessionData() = 0;
	QSharedPointer<AnalysisValue> getScriptValue(int index);
	//Inherited
	virtual QString defaultTagName(){return "AnalysisDataSource";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QSqlDatabase session_;

private:
	struct TriggeredAnalysisValue
	{
		TriggeredAnalysisValue(EventOrderIndex triggeredId,QSharedPointer<AnalysisValue> val){triggeredIndex=triggeredId;value=val;};
		EventOrderIndex triggeredIndex;
		QSharedPointer<AnalysisValue> value;
	};
	QLinkedList<QSharedPointer<TriggeredAnalysisValue>> values_;
	QVector<QSharedPointer<TriggeredAnalysisValue>> scriptValues_;
	unsigned int usefulScriptValues_;

};

}; //namespace Picto
#endif