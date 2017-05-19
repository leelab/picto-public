#include <QFileInfo>
#include <QPainter>
#include <QProcess>

#include "RecordingVisualTargetHost.h"
#include "../../common/iodevices/BufferFileGenerator.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

#define BITRATE 10000000
#define GOP 300
#define FPS 60
#define VIDEOTYPE "avi"	//QtFFMpegWrapper supported types should be (but in testing, not so sure): *.avi *.asf *.mpg *.wmv

RecordingVisualTargetHost::RecordingVisualTargetHost()
	: VisualTargetHost()
{
	recording_ = false;
	recordingInitialized_ = false;
	setRecordingTime(-1);
	frameTimer_.start();
	
	selectedChannel_ = 0;
	selectedUnit_ = 0;

}

RecordingVisualTargetHost::~RecordingVisualTargetHost()
{
	restartRecording();
}

/*! \brief Returns whether this object is currently in recording mode.
 */
bool RecordingVisualTargetHost::isRecording()
{
	return recording_;
}

/*! \brief Saves the recorded file to the input file path.  Overwrites anything already at that path.
 *	Returns false if the file could not be saved at the path.
 */
bool RecordingVisualTargetHost::saveRecordingAs(QString filePath)
{
	//Check if we have a video file
	if(!videoFile_)
		return false;

	//Since QFile::Copy won't overwrite anything, remove the file at
	//the filepath first
	if (QFile::exists(filePath) && !QFile::remove(filePath))
		return false;
	//Finish up the recording (current MediaMuxer needs to be closed for the file to be valid)
	finishRecording();

	//Copy the temporary video file to the filepath
	if(!videoFile_->copy(filePath))
		return false;
	return true;
}

/*! \brief  Returns the type of video file currently handled by this object (ie. "avi" or "mpg").
 */
QString RecordingVisualTargetHost::getVideoFileType()
{
	return VIDEOTYPE;
}

/*! \brief Toggles the recording state.  
 *	\details When recording is on, each paint event causes a frame to 
 *	be written to video.  When recording is off frames can be displayed 
 *	to the screen without going into the video file.
 */
void RecordingVisualTargetHost::toggleRecording()
{
	if(recording_)
	{
		recording_ = false;
		updateRecordingTime();
	}
	else
	{
		recording_ = true;
		//Restart the frame timer
		frameTimer_.restart();
	}
}

/*! \brief Closes the video file and zeros out the recording time.  Any new recording will erase
 *	the previous video file and start a new one.
 */
void RecordingVisualTargetHost::restartRecording()
{
	//Finish up the recording
	finishRecording();
	
	//Reinitialize recording time to -1.
	setRecordingTime(-1);
}

/*! \brief Does normal inherited painting plus optional recording of visual target data to a video.
 */
void RecordingVisualTargetHost::paintEvent(QPaintEvent* e)
{
	//Do inherited painting
	VisualTargetHost::paintEvent(e);

	//Leave if recording mode is disabled
	if(!recording_)
		return;

	//Get visual target and dimensions
	QSharedPointer<VisualTarget> target = getVisualTarget();
	if(target.isNull())
		return;
	QRect targetRect = target->getDimensions();

	//If recording wasn't initialized, intialize it
	if(!recordingInitialized_)
	{
		recordingInitialized_ = initializeRecording();
	}
	Q_ASSERT(encoder_);
	
	//Update the recording time
	updateRecordingTime();

	//Write the current frame data to a frame image
	QImage frame(targetRect.width(),targetRect.height(),QImage::Format_RGB32);
	target->paint(&frame);
	//Write the frame image to the file at the updated recording time
	encoder_->encodeImagePts(frame,getRecordingTime());
}
void RecordingVisualTargetHost::rewarded(int quantity)
{
	if (encoder_)
	{
		//Update the recording time
		updateRecordingTime();

		//for (int index = 0; index < quantity; index++)
		{
			//double newtime = getRecordingTime () - elapsedTime;
			double time = 0.;
			//unsigned pts = getRecordingTime();
			encoder_->encodeAudioFrame(getRecordingTime(), time, true);
		}
	}
}
void RecordingVisualTargetHost::spikeAdded(double spikeTime)
{
	if (encoder_)
	{
		//Update the recording time
		updateRecordingTime();

		//double newtime = getRecordingTime () - elapsedTime;
		double time = spikeTime * 1000.; //ms
		encoder_->encodeAudioFrame(getRecordingTime(), time, false);		
	}
}
/*! \brief Recording needs to be initialized once before any video files are recorded.  This function
 *	takes care of that.
*	\details Essentially what we are doing here is setting up the MediaMuxer from the
*	QTFFMpegWrapper package.
*/
bool RecordingVisualTargetHost::initializeRecording()
{
	//setup frame image
	QSharedPointer<VisualTarget> target = getVisualTarget();
	if(target.isNull())
		return false;
	QRect targetRect = target->getDimensions();
		
	//Reinitialize recording time
	setRecordingTime(-1);

	//reinitialize encoder
	encoder_ = QSharedPointer<MediaMuxer>(new MediaMuxer());
	if(!encoder_)
		return false;

	//If we were using a file, erase it
	if(videoFile_)
	{
		videoFile_->remove();
	}
	
	//Create a new temporary video file
	videoFile_ = BufferFileGenerator::createTempFile("ExpVideos",VIDEOTYPE);
	if(!videoFile_)
		return false;

	//initialize encoder with the video file
	if(!encoder_->createFile(QFileInfo(*(videoFile_.data())).absoluteFilePath(),targetRect.width(),targetRect.height(),BITRATE*(1000/FPS),GOP,1000))
		return false;

	return true;
}
void RecordingVisualTargetHost::setSelectedNeural(int channel, int unit)
{
	selectedChannel_ = channel;
	selectedUnit_ = unit;
}
/*! \brief Finishes up recording and closes required resources.
 */
void RecordingVisualTargetHost::finishRecording()
{
	//Close the encoder
	if(encoder_)
	{
		//Close the encoder file
		encoder_->close();

		//Delete the encoder and frame image
		encoder_.clear();
	}
	
	//Mark recording as uninitialized
	recordingInitialized_ = false;
}

/*! \brief Updates the current recording time to the current time.
 *	\details Prioer record state toggle events and initialization conditions are taken 
 *	into account in the current time calculation.  Time is actually set using setRecordingTime().
 */
void RecordingVisualTargetHost::updateRecordingTime()
{
	//Get the last recording time
	double lastFrameTime = getRecordingTime();
	//If it was less than zero, the new time will be zero
	//Otherwise, new time equals last time plus the time on the frame timer clock
	double newFrameTime = 0;
	if(lastFrameTime >= 0)
	{
		newFrameTime = lastFrameTime + frameTimer_.elapsed();
	}
	//Restart the frame timer clock
	frameTimer_.restart();

	//Store the new recording time
	setRecordingTime(newFrameTime);
}

/*! \brief Gets the latest recording time in Ms
 */
int RecordingVisualTargetHost::getRecordingTime()
{
	return currFrameMs_;
}
void RecordingVisualTargetHost::updateFrameTime(double time)
{
	int MsTime = (int)time;
	setRecordingTime(MsTime);
}


/*! \brief Sets the current recording time to a new value and tells any connected objects who care.
 */
void RecordingVisualTargetHost::setRecordingTime(int currMs)
{
	//If time didn't change, do nothing
	if(currFrameMs_ == currMs)
		return;
	//Update the stored current frame time value
	currFrameMs_ = currMs;
	//If the new current frame time value is less than zero, it is an
	//initialization value, emit a time of zero to listeners
	if(currFrameMs_ < 0)
	{
		emit updateRecordingTime(0);
		return;
	}
	//Otherwise, emit the current time in seconds to listeners
	emit updateRecordingTime(double(currFrameMs_)/1000.0);
}