
#ifndef _RECORDING_VISUAL_TARGET_HOST_H_
#define _RECORDING_VISUAL_TARGET_HOST_H_

#include <QImage>
#include <QSharedPointer>
#include <QFile>
#include <QTime>
#include "../../common/compositor/VisualTargetHost.h"
#include "QVideoEncoder.h"

namespace Picto {

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
