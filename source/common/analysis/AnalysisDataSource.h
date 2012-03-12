#ifndef _ANALYSIS_DATA_SOURCE_H_
#define _ANALYSIS_DATA_SOURCE_H_

#include <QVariant>
#include <QScriptValue>
#include <QSqlDatabase>
#include "EventOrderIndex.h"
#include "../statemachine/UIEnabled.h"

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

	//Should be extended by child classes to return their value at the input time.  The input
	//value to this function will always increase until restart is called.  This means that 
	//child classes can store previous values and iterate forward to get the value at a new 
	//time rather than always restarting their search at zero.
	virtual QString getValue(const EventOrderIndex& index) = 0;

	//Inherited
	virtual QString getUITemplate(){return "DataSource";};
	virtual QString assetType(){return "DataSource";};

protected:

	//Inherited
	virtual QString defaultTagName(){return "AnalysisDataSource";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QSqlDatabase session_;

};
}; //namespace Picto
#endif