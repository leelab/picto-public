#include "SpikeState.h"
#include "PlaybackData.h"
using namespace Picto;


void SpikeState::setDatabase(QSqlDatabase session)
{

}

void SpikeState::startRun(double runStartTime,double runEndTime)
{
}

PlaybackIndex SpikeState::getCurrentIndex()
{
	return PlaybackIndex();
}

PlaybackIndex SpikeState::getNextIndex(double lookForwardTime)
{
	return PlaybackIndex();
}

void SpikeState::moveToIndex(PlaybackIndex index)
{
}