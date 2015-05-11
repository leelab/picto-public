#ifndef _CURRENT_SESSION_DATA_H_
#define _CURRENT_SESSION_DATA_H_

#include <QVariantList>
#include <QMutex>
#include <QSharedPointer>
#include <QPair>

#include "SessionData.h"

/*!	\brief Stores all of the current values for each piece of session data defining the current Experimental state.
 *	\details This object is used to provide the current Experimental state to Workstations that are displaying Experiment
 *	activity to an Operator.  The updateVariable() and updateVariables() functions are used to update the stored variables
 *	with their latest values.  Then data can be copied or moved out by using copyDataTo() or moveDataTo().  When these
 *	functions are called, enter a time double into the condition variables.  All pieces of session data whose values
 *	changed after that time will be copied/moved.
 *
 *	Internally, this object maintains two lookup tables.  The first table stores all session data keyed by its VariableId.
 *	When new data comes in, the value in that table for its VariableID is either updated or not depending on whether the
 *	new data is newer than the stored data or not.  The other lookup table stores data keyed by the time at which it
 *	changed.  This is useful since the object needs to return all data that occured after a specific time, and this
 *	prevent us from having to sort lots of data every time this type of read occurs.  Updating of this second table occurs
 *	every time the first table receives an update.  Whenever an entry in the first table is replaced, the replaced entry
 *	is removed from the second table, and the new entry is added to the second table with the key of its new timestamp.
 *
 *	\note This object is meant to be used with SerialSessionData.  By copying data from this object to a SerialSessionData
 *	object, we can very easily serialize all data needed by a Workstaion whenever requests come in for the latest
 *	Experimental state.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class CurrentSessionData : public SessionData
{
public:
	CurrentSessionData::CurrentSessionData(int dataType);
	virtual ~CurrentSessionData();

	void updateVariable(int dataid,int variableId,QString timestamp,QString data);
	void updateVariables(QList<QVariantList> data);

protected:
	void writeData(int dataType, QVariantList data);
	QList<int>readDataTypes();
	QList<QVariantList> readData(int dataType,QVariant condition,bool cut=false);
	virtual void eraseEverything();

private:
	QMap<int,QVariantList> dataMap_;
	QMap<double,QMap<int,int>> dataByTime_;
	int dataType_;
	QMap<int,int> lastOffset_;
};

#endif

