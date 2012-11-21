#include <QApplication>
#include "SessionLoader.h"
using namespace Picto;

SessionLoader::SessionLoader(QSharedPointer<SessionState> sessState) :
sessionState_(sessState)
{
	connect(sessionState_.data(),SIGNAL(needsData(PlaybackDataType,PlaybackIndex,PlaybackIndex)),this,SLOT(loadData(PlaybackDataType,PlaybackIndex,PlaybackIndex)));

	connect(sessionState_.data(),SIGNAL(needsNextData(PlaybackDataType,PlaybackIndex,bool)),this,SLOT(loadNextData(PlaybackDataType,PlaybackIndex,bool)));
}

SessionLoader::~SessionLoader()
{
}

void SessionLoader::loadData(PlaybackDataType type,PlaybackIndex currLast,PlaybackIndex to)
{
	Q_ASSERT(!loadProcesses_[type].isRunning());
	emit loading(true);
	childLoadData(type,currLast,to);
	//loadProcesses_[type] = QtConcurrent::run(this,&SessionLoader::childLoadData,type,currLast,to);
	//while(loadProcesses_[type].isRunning())
	//	QCoreApplication::processEvents();
	emit loading(false);
}

void SessionLoader::loadNextData(PlaybackDataType type,PlaybackIndex currLast,bool backward)
{
	Q_ASSERT(!loadProcesses_[type].isRunning());
	emit loading(true);
	childLoadNextData(type,currLast,backward);
	//loadProcesses_[type] = QtConcurrent::run(this,&SessionLoader::childLoadNextData,type,currLast,backward);
	//while(loadProcesses_[type].isRunning())
	//	QCoreApplication::processEvents();
	emit loading(false);
}