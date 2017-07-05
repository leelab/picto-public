
#ifndef _RECORDING_VISUAL_TARGET_HOST_H_
#define _RECORDING_VISUAL_TARGET_HOST_H_

#include <QImage>
#include <QSharedPointer>
#include <QFile>
#include <QTime>
#include "../../common/compositor/VisualTargetHost.h"
#include "../../common/compositor/MediaMuxer.h"

namespace Picto {

/*! \brief This adds video file recording functionality onto the regular VisualTargetHost functionality.
 *	\details This class extends the VisualTargetHost so that whenever a new frame is drawn to its VisualTarget
 *	it is also writtent to a video file.  Functions are provided for enabling/disabling the "copy frame to 
 *	video file" functionality.  saveRecordingAs() can be used to save the recorded video file to a particular
 *	path on disk.
 *	\note This class currently uses qtffmpegwrapper which is a wrapper for ffmpeg since there is no way yet to 
 *	save video files directly from Qt.  qtffmpegwrapper will undoubtedly need to be updated at some point.  It can
 *	be found in Picto\\3rdParty.
	*	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
	*	\date 2009-2017
	*/
class RecordingVisualTargetHost : public VisualTargetHost
{
	Q_OBJECT
public:
	RecordingVisualTargetHost();
	~RecordingVisualTargetHost();

		bool isRecording();
		bool saveRecordingAs(QString filePath);
		QString getVideoFileType();
		
		int getRecordingTime();
		//neural data sonification:
		void rewarded(int quantity);
		void spikeAdded(double spikeTime);
		
	signals:
		/*! \brief Emitted when the current recording time changes.  currTime is in seconds.*/
		void updateRecordingTime(double currTime);
		public slots:
		void toggleRecording();
		void restartRecording();
		void updateFrameTime(double);
		
	protected:
		virtual void paintEvent(QPaintEvent *e);


	private:
		bool initializeRecording();
		void finishRecording();
		void updateRecordingTime();
		void setRecordingTime(int currMs);
		QSharedPointer<MediaMuxer> encoder_;
		bool recording_;
		bool recordingInitialized_;
		int currFrameMs_;
		QTime frameTimer_;
		QSharedPointer<QFile> videoFile_;
	
		int selectedChannel_;
		int selectedUnit_;
	};


}; //namespace Picto

#endif
