#include <QApplication>
#include "TestPlaybackController.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

/*! \brief Constructs a TestPlaybackController object.
 *	@param experiment The Experiment that will be tested.
 */
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

/*! \brief Sets the task to be played back when play() is next called.
*/
void TestPlaybackController::setTask(QString taskName)
{
	taskName_ = taskName;
}

/*! \brief Returns true if a Task is running.
*/
bool TestPlaybackController::isRunning()
{
	bool isRunning = running_;
	return isRunning;
}

/*! \brief Starts / Resume playback of the current Task.
 *	\details When no Task is running, this emits running() then starts running the Task.
 *	The function blocks until the Task ends, either on its own, or because stop was pressed.
 *	When that happens, the stopped() signal is emitted and the function ends.
 *	
 *	If this is called when a Task is already running, it causes the task to resume, and
 *	the running() signal is emitted.
 *	\note Since the Task runs underneath the play() function, when this function is used
 *	to resume playback, it is actually being called from below another call to play().  This
 *	is confusing, and at some point we need to refactor the Experimental run system so that
 *	it is played from within a Qt Event loop.
 */
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

/*! \brief Pauses the currently running Task and emits the paused() signal.
*/
void TestPlaybackController::pause()
{
	if(!running_ || paused_)
		return;
	experiment_->getEngine()->pause();
	emit paused();
	paused_ = true;
}

/*! \brief Stops the currently running task.
 *	\details As described in play(), if a Task is running it means that this is getting
 *	called somewhere underneath a call to TestPlaybackController::play().  The reults of
 *	this function will be that the Experiment::runTask() function in play() will return, 
 *	and then execution will continue within that function until the stopped() signal is emitted.
 */
void TestPlaybackController::stop()
{
	if(!running_)
		return;
	experiment_->getEngine()->stop();
	paused_ = false;
	stopping_ = true;
}