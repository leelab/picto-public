#ifndef _AnalysisRunNotesData_H_
#define _AnalysisRunNotesData_H_

#include <QWeakPointer>
#include "../common.h"

#include "AnalysisDataSource.h"

namespace Picto {

/*!	\brief A parameter for retrieving frame times.
 *
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisRunNotesData : public AnalysisDataSource
#else
class AnalysisRunNotesData : public AnalysisDataSource
#endif
{
	Q_OBJECT
public:
	AnalysisRunNotesData();
	virtual ~AnalysisRunNotesData(){};

	static QSharedPointer<Asset> Create();

	virtual void enteredScope();

	virtual QString friendlyTypeName(){return "Run Info";};
	virtual QString getUITemplate(){return "RunNotesDataSource";};
	virtual QString getUIGroup(){return "Sensors";};

public slots:
	//Gets the name of this run, either the default name or the one set by the operator in the run info section
	//during a session.
	QString getRunName();
	//Returns the time that this run started where time zero is the beginning of the session
	double getGlobalStartTime();
	//Returns the time that this run ends where time zero is the beginning of the session
	double getGlobalEndTime();
	//Returns the notes string that was saved by the operator during the session.  If no notes were saved, an empty
	//string is returned.
	QString getRunNotes();
	//Returns the index of this run in this sessions list of task runs, starting with zero.
	int getRunIndex();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


};


}; //namespace Picto

#endif
