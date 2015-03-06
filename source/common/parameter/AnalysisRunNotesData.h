#ifndef _AnalysisRunNotesData_H_
#define _AnalysisRunNotesData_H_

#include <QWeakPointer>
#include "../common.h"

#include "AnalysisDataSource.h"

namespace Picto {

/*!	\brief An AnalysisDataSource for querying meta-data about the current run.
 *	\details This object can be used to query the name, start time, end time, notes
 *	and session index of the current run.  The name and run notes are set by the 
 *	operator during a session in the workstation's run info tab and can include information
 *	about the chamber, electrode position used, etc.  
 *
 *	The AnalysisRunNotesData object is useful for saving this metadata about a particular 
 *	run to file alongside other analyzed data from that run so that we can be sure that the 
 *	context in which a session occured is available with the analyzed data from that session.
 *	
 *	\note In the future, we would like to be able to add additional fields to the run notes
 *	to make it easier to query metadata about a particular session over multiple sessions.  
 *	When that happens, this class will have to be modified for accessing that data as well.
 *
 *	\note This class uses a RunNotesReader as its main data source.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	QString getRunName();
	double getGlobalStartTime();
	double getGlobalEndTime();
	QString getRunNotes();
	int getRunIndex();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


};


}; //namespace Picto

#endif
