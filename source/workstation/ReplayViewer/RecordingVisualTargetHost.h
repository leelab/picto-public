
#ifndef _RECORDING_VISUAL_TARGET_HOST_H_
#define _RECORDING_VISUAL_TARGET_HOST_H_

#include <QImage>
#include <QSharedPointer>
#include <QFile>
#include <QTime>
#include "../../common/compositor/VisualTargetHost.h"
#include "QVideoEncoder.h"

namespace Picto {

/*! \brief This adds video file recording functionality onto the regular VisualTargetHost functionality.
 *	\details This class extends the VisualTargetHost so that whenever a new frame is drawn to its VisualTarget
 *	it is also writtent to a video file.  Functions are provided for enabling/disabling the "copy frame to 
 *	video file" functionality.  saveRecordingAs() can be used to save the recorded video file to a particular
 *	path on disk.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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
signals:
	/*! \brief Emitted when the current recording time changes.  currTime is in seconds.*/
	void updateRecordingTime(double currTime);
public slots:
	void toggleRecording();
	void restartRecording();
protected:
	virtual void paintEvent(QPaintEvent *e);
private:
	bool initializeRecording();
	void finishRecording();
	void updateRecordingTime();
	int getRecordingTime();
	void setRecordingTime(int currMs);
	QSharedPointer<QVideoEncoder> encoder_;
	bool recording_;
	bool recordingInitialized_;
	int currFrameMs_;
	QTime frameTimer_;
	QSharedPointer<QFile> videoFile_;
};


}; //namespace Picto

#endif
