#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "SessionLoader.h"
using namespace Picto;

SessionLoader::SessionLoader(QSharedPointer<SessionState> sessState) :
sessionState_(sessState)
{
}

SessionLoader::~SessionLoader()
{
}

//Note: With signals, the definition is such that offsetTime after the frameTime of frameId is when the first signal data was read.

bool SessionLoader::setFile(QString path)
{
	return true;
}
QStringList SessionLoader::getRunNames()
{
	return QStringList();
}
bool SessionLoader::loadRun(int runIndex,double maxTime)
{
	sessionState_->reset();
	int dataId = 0;
	for(int i=0;i<100;i++)
	{
		sessionState_->setTransition(i,++dataId,i);
		for(int j=0;j<10;j++)
			sessionState_->setPropertyValue(i,++dataId,j,QString("%1 Prop Val %2").arg(j).arg(i));
		sessionState_->setFrame(++dataId,i);
	}
	return true;
}