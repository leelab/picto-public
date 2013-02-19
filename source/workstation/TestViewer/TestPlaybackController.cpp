#include <QApplication>
#include "TestPlaybackController.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

TestPlaybackController::TestPlaybackController(QSharedPointer<Experiment> experiment)
{
	experiment_ = experiment;
	running_ = false;
	paused_ = false;
	stopping_ = false;
}

TestPlaybackController::~TestPlaybackController()
{
	stop();
}

void TestPlaybackController::setTask(QString taskName)
{
	taskName_ = taskName;
}

bool TestPlaybackController::isRunning()
{
	bool isRunning = running_;
	return isRunning;
}

void TestPlaybackController::play()
{
	bool isRunning = running_;
	if(isRunning)
	{
		if(!paused_)
			return;
		experiment_->getEngine()->play();
		paused_ = false;
		emit running();
		return;
	}
	paused_ = false;
	running_ = true;
	QString taskName = taskName_;
	Q_ASSERT(taskName.size());
	emit running();
	experiment_->runTask(taskName);
	running_ = false;
	stopping_ = false;
	emit stopped();
}

void TestPlaybackController::pause()
{
	if(!running_ || paused_)
		return;
	experiment_->getEngine()->pause();
	emit paused();
	paused_ = true;
}

void TestPlaybackController::stop()
{
	if(!running_)
		return;
	experiment_->getEngine()->stop();
	paused_ = false;
	stopping_ = true;
}