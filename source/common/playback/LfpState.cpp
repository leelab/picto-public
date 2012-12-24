#include "LfpState.h"
#include "PlaybackData.h"
using namespace Picto;

LfpState::LfpState(int channel,double sampPeriod) :
channel_(channel),
sampPeriod_(sampPeriod)
{

}

void LfpState::setDatabase(QSqlDatabase session)
{

}

void LfpState::startRun(double runStartTime,double runEndTime)
{
}

PlaybackIndex LfpState::getCurrentIndex()
{
	return PlaybackIndex();
}

PlaybackIndex LfpState::getNextIndex(double lookForwardTime)
{
	return PlaybackIndex();
}

void LfpState::moveToIndex(PlaybackIndex index)
{
}

