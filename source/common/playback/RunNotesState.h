#ifndef _RunNotesState_H_
#define _RunNotesState_H_
#include <QVector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSharedPointer>
#include "PlaybackInterfaces.h"

namespace Picto {
/*! \brief Stores Run Notes for use in Playback system.
 */
class RunNotesState : public RunNotesReader
{
	Q_OBJECT
public:
	virtual void setDatabase(QSqlDatabase session);
	virtual void startRun(double runStartTime,double runEndTime = -1);
	virtual QString getName();
	virtual double getStartTime();
	virtual double getEndTime();
	virtual QString getNotes();
	virtual int getRunIndex();

private:
	bool currRunIsValid();
	QSqlDatabase session_;
	QSharedPointer<QSqlQuery> query_;
	struct RunInfo
	{
		RunInfo(){name="";index=-1;notes="";startTime=-1;endTime=-1;};
		QString name;
		int index;
		QString notes;
		double startTime;
		double endTime;
	};
	QVector<RunInfo> runs_;
	int currRun_;
};

}; //namespace Picto
#endif