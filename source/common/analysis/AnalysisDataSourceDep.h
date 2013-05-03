#ifndef _ANALYSIS_DATA_SOURCE_H_
#define _ANALYSIS_DATA_SOURCE_H_

#include <QVariant>
#include <QScriptValue>
#include <QScriptEngine>
#include <QSqlDatabase>
#include <QLinkedList>
#include "AnalysisDataIterator.h"
#include "EventOrderIndex.h"
#include "../statemachine/UIEnabled.h"
#include "AnalysisValue.h"

namespace Picto {
#if defined WIN32 || defined WINCE
class PICTOLIB_API AnalysisDataSourceDep : public UIEnabled
#else
class AnalysisDataSourceDep : public UIEnabled
#endif
{
	Q_OBJECT
public:
	AnalysisDataSourceDep();
	virtual ~AnalysisDataSourceDep();

	void loadSessionAndScriptTools(QSqlDatabase session,QSharedPointer<QScriptEngine> qsEngine,QScriptValue parent);
	void setDataWindow(EventOrderIndex startFrom,EventOrderIndex endBefore);


	//AnalysisDataSourceDep specific functions
	//Resets to an initial state.
	//In particular, getValue(time) will always have an increasing input
	//time until reset is called.
	//void reset();
	void sessionDatabaseUpdated();

	void storeValue(QScriptValue triggerScript,const EventOrderIndex& index);

	//Returns the Data Source value at the input time.  The input
	//value to this function is assumed to always increase until reset is called.
	QSharedPointer<AnalysisValue> getValue(const EventOrderIndex& index);
	QString getIteratorDescriptor();

	//Inherited
	virtual QString getUITemplate(){return "DataSource";};
	virtual QString friendlyTypeName(){return "DataSource";};

protected:

	virtual QSharedPointer<AnalysisDataIterator> createDataIterator(){return QSharedPointer<AnalysisDataIterator>();};
	virtual void fillOutScriptValue(QSharedPointer<AnalysisValue> val);
	QScriptValue createScriptArray(unsigned int length=0);
	void setScriptInfo(QString name,QScriptValue value);
	//Inherited
	virtual QString defaultTagName(){return "AnalysisDataSource";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QSqlDatabase session_;
	QSharedPointer<QScriptEngine> qsEngine_;
	QScriptValue parentScript_;

private:
	QSharedPointer<AnalysisDataIterator> dataIterator_;
	bool parentUsesSameIterator_;
	QSharedPointer<AnalysisValue> nextValue_;
	QSharedPointer<AnalysisValue> latestValue_;

};

}; //namespace Picto
#endif