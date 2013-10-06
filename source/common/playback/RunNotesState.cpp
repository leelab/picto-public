#include "RunNotesState.h"
using namespace Picto;

void RunNotesState::setDatabase(QSqlDatabase session)
{
	session_ = session;
	query_ = QSharedPointer<QSqlQuery>(new QSqlQuery(session_));
	query_->exec("SELECT r.name,r.notes, fs.time, fe.time FROM runs r, frames fs, frames fe WHERE fs.dataid = r.firstframe AND fe.dataid = r.lastframe");
	if(!query_->exec())
	{
		Q_ASSERT(false);
		return;
	}
	runs_.clear();
	while(query_->next())
	{
		RunInfo runInfo;
		runInfo.name = query_->value(0).toString();
		runInfo.index = runs_.size();
		runInfo.notes = query_->value(1).toString();
		runInfo.startTime = query_->value(2).toDouble();
		runInfo.endTime = query_->value(3).toDouble();
		runs_.append(runInfo);
	}
	currRun_ = -1;
}

void RunNotesState::startRun(double runStartTime,double)
{
	Q_ASSERT(session_.isOpen());
	int foundIndex = -1;
	for(int i=0;i<runs_.size();i++)
	{
		double storedStartTime = runs_[i].startTime;
		if(runStartTime < runs_[i].startTime - .1)	//-.01 is in case of floating point errors (we assume that two runs can't be started with 100ms of each other
			break;
		foundIndex = i;
	}
	currRun_ = foundIndex;
}

QString RunNotesState::getName()
{
	if(!currRunIsValid())
		return "";
	return runs_[currRun_].name;
}

double RunNotesState::getStartTime()
{
	if(!currRunIsValid())
		return -1;
	return runs_[currRun_].startTime;
}

double RunNotesState::getEndTime()
{
	if(!currRunIsValid())
		return -1;
	return runs_[currRun_].endTime;
}

QString RunNotesState::getNotes()
{
	if(!currRunIsValid())
		return "";
	return runs_[currRun_].notes;
}

int RunNotesState::getRunIndex()
{
	if(!currRunIsValid())
		return -1;
	return runs_[currRun_].index;
}

bool RunNotesState::currRunIsValid()
{
	if(currRun_ < 0 || currRun_ >= runs_.size())
		return false;
	return true;
}