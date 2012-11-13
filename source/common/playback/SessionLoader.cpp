#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include "SessionLoader.h"
using namespace Picto;

SessionLoader::SessionLoader(QSharedPointer<SessionState> sessState) :
sessionState_(sessState),
bufferTime_(0.1),
tempId_(1)
{
	connect(sessionState_.data(),SIGNAL(needsPropertyData(PlaybackIndex,PlaybackIndex)),this,SLOT(loadPropertyData(PlaybackIndex,PlaybackIndex)));
	connect(sessionState_.data(),SIGNAL(needsTransitionData(PlaybackIndex,PlaybackIndex)),this,SLOT(loadTransitionData(PlaybackIndex,PlaybackIndex)));
	connect(sessionState_.data(),SIGNAL(needsFrameData(PlaybackIndex,PlaybackIndex)),this,SLOT(loadFrameData(PlaybackIndex,PlaybackIndex)));
	connect(sessionState_.data(),SIGNAL(needsRewardData(PlaybackIndex,PlaybackIndex)),this,SLOT(loadRewardData(PlaybackIndex,PlaybackIndex)));
	connect(sessionState_.data(),SIGNAL(needsSignalData(PlaybackIndex,PlaybackIndex)),this,SLOT(loadSignalData(PlaybackIndex,PlaybackIndex)));
	connect(sessionState_.data(),SIGNAL(needsLFPData(PlaybackIndex,PlaybackIndex)),this,SLOT(loadLFPData(PlaybackIndex,PlaybackIndex)));
	connect(sessionState_.data(),SIGNAL(needsSpikeData(PlaybackIndex,PlaybackIndex)),this,SLOT(loadSpikeData(PlaybackIndex,PlaybackIndex)));

	connect(sessionState_.data(),SIGNAL(needsNextPropertyData(PlaybackIndex,bool)),this,SLOT(loadNextPropertyData(PlaybackIndex,bool)));
	connect(sessionState_.data(),SIGNAL(needsNextTransitionData(PlaybackIndex,bool)),this,SLOT(loadNextTransitionData(PlaybackIndex,bool)));
	connect(sessionState_.data(),SIGNAL(needsNextFrameData(PlaybackIndex,bool)),this,SLOT(loadNextFrameData(PlaybackIndex,bool)));
	connect(sessionState_.data(),SIGNAL(needsNextRewardData(PlaybackIndex,bool)),this,SLOT(loadNextRewardData(PlaybackIndex,bool)));
	connect(sessionState_.data(),SIGNAL(needsNextSignalData(PlaybackIndex,bool)),this,SLOT(loadNextSignalData(PlaybackIndex,bool)));
	connect(sessionState_.data(),SIGNAL(needsNextLFPData(PlaybackIndex,bool)),this,SLOT(loadNextLFPData(PlaybackIndex,bool)));
	connect(sessionState_.data(),SIGNAL(needsNextSpikeData(PlaybackIndex,bool)),this,SLOT(loadNextSpikeData(PlaybackIndex,bool)));


	int numFrames = 60;
	transPerFrame_ = 5;
	propPerFrame_ = 3000;
	double frameLength = 0.01;
	qulonglong dataId = 0;
	double frameTime;
	props_.resize(numFrames*propPerFrame_);
	trans_.resize(numFrames*transPerFrame_);
	frames_.resize(numFrames);
	for(int i=0;i<numFrames;i++)
	{
		frameTime = frameLength*double(i);
		for(int j=0;j<transPerFrame_;j++)
		{
			trans_[i*transPerFrame_+j].first = frameTime;
			trans_[i*transPerFrame_+j].second = ++dataId;
		}
		for(int j=0;j<propPerFrame_;j++)
		{
			props_[i*propPerFrame_+j].first = frameTime;
			props_[i*propPerFrame_+j].second = ++dataId;
		}
		frames_[i].first = frameTime;
		frames_[i].second = ++dataId;
	}
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
	return true;
}

void SessionLoader::loadPropertyData(PlaybackIndex currLast,PlaybackIndex to)
{
	int i;
	if(to < currLast)
	{
		for(i=props_.size()-1;i>=0 && props_[i].first >= currLast.time();i--);
		for(;i>=0 && props_[i].first >= to.time();i--)
			sessionState_->setPropertyValue(props_[i].first,props_[i].second,i%propPerFrame_,QString("Time:%1,ID:%2").arg(props_[i].first).arg(props_[i].second));
	}
	else
	{
		for(i=0;i<props_.size() && props_[i].first <= currLast.time();i++);
		for(;i<props_.size() && props_[i].first <= to.time();i++)
			sessionState_->setPropertyValue(props_[i].first,props_[i].second,i%propPerFrame_,QString("Time:%1,ID:%2").arg(props_[i].first).arg(props_[i].second));
	}
}
void SessionLoader::loadTransitionData(PlaybackIndex currLast,PlaybackIndex to)
{
	int i;
	if(to < currLast)
	{
		for(i=trans_.size()-1;i>=0 && trans_[i].first >= currLast.time();i--);
		for(;i>=0 && trans_[i].first >= to.time();i--)
			sessionState_->setTransition(trans_[i].first,trans_[i].second,i%transPerFrame_);
	}
	else
	{
		for(i=0;i<trans_.size() && trans_[i].first <= currLast.time();i++);
		for(;i<trans_.size() && trans_[i].first <= to.time();i++)
			sessionState_->setTransition(trans_[i].first,trans_[i].second,i%transPerFrame_);
	}
}
void SessionLoader::loadFrameData(PlaybackIndex currLast,PlaybackIndex to)
{
	int i;
	if(to < currLast)
	{
		for(i=frames_.size()-1;i>=0 && frames_[i].first >= currLast.time();i--);
		for(;i>=0 && frames_[i].first >= to.time();i--)
			sessionState_->setFrame(frames_[i].second,frames_[i].first);
	}
	else
	{
		for(i=0;i<frames_.size() && frames_[i].first <= currLast.time();i++);
		for(;i<frames_.size() && frames_[i].first <= to.time();i++)
			sessionState_->setFrame(frames_[i].second,frames_[i].first);
	}
}
void SessionLoader::loadRewardData(PlaybackIndex currLast,PlaybackIndex to)
{
}
void SessionLoader::loadSignalData(PlaybackIndex currLast,PlaybackIndex to)
{
}
void SessionLoader::loadLFPData(PlaybackIndex currLast,PlaybackIndex to)
{
}
void SessionLoader::loadSpikeData(PlaybackIndex currLast,PlaybackIndex to)
{
}





void SessionLoader::loadNextPropertyData(PlaybackIndex currLast,bool backward)
{	

	int i;
	if(backward)
	{
		for(i=props_.size()-1;i>=0 && props_[i].first >= currLast.time();i--);
		if(i<0)
			return;
		sessionState_->setPropertyValue(props_[i].first,props_[i].second,i%propPerFrame_,QString("Time:%1,ID:%2").arg(props_[i].first).arg(props_[i].second));
	}
	else
	{
		for(i=0;i<props_.size() && props_[i].first <= currLast.time();i++);
		if(i == props_.size())
			return;
		sessionState_->setPropertyValue(props_[i].first,props_[i].second,i%propPerFrame_,QString("Time:%1,ID:%2").arg(props_[i].first).arg(props_[i].second));
	}
}
void SessionLoader::loadNextTransitionData(PlaybackIndex currLast,bool backward)
{	
	int i;
	if(backward)
	{
		for(i=trans_.size()-1;i>=0 && trans_[i].first >= currLast.time();i--);
		if(i<0)
			return;
		sessionState_->setTransition(trans_[i].first,trans_[i].second,i%transPerFrame_);
	}
	else
	{
		for(i=0;i<trans_.size() && trans_[i].first <= currLast.time();i++);
		if(i == trans_.size())
			return;
		sessionState_->setTransition(trans_[i].first,trans_[i].second,i%transPerFrame_);
	}
}
void SessionLoader::loadNextFrameData(PlaybackIndex currLast,bool backward)
{
	int i;
	if(backward)
	{
		for(i=frames_.size()-1;i>=0 && frames_[i].first >= currLast.time();i--);
		if(i<0)
			return;
		sessionState_->setFrame(frames_[i].second,frames_[i].first);
	}
	else
	{
		for(i=0;i<frames_.size() && frames_[i].first <= currLast.time();i++);
		if(i == frames_.size())
			return;
		sessionState_->setFrame(frames_[i].second,frames_[i].first);
	}
}
void SessionLoader::loadNextRewardData(PlaybackIndex currLast,bool backward)
{
}
void SessionLoader::loadNextSignalData(PlaybackIndex currLast,bool backward)
{
}
void SessionLoader::loadNextLFPData(PlaybackIndex currLast,bool backward)
{
}
void SessionLoader::loadNextSpikeData(PlaybackIndex currLast,bool backward)
{
}