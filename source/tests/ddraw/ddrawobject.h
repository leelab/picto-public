#ifndef DDRAWOBJECT_H
#define DDRAWOBJECT_H

#include <QWidget>
#include <QTimer>
#include <QtScript>
#include <QtNetwork>
#include <QThread>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <ddraw.h>

class DDrawObject : public QWidget
{
	Q_OBJECT

private slots:

	void flipBuffer();

public:
	DDrawObject(QWidget *parent = 0);
	~DDrawObject();

	void setScript(QString newPositionScript);

protected:

	void keyPressEvent(QKeyEvent * keyEvent);

private:

	void UpdateFrame();

	int timeDelta, lastFrameTime;

	QScriptEngine scriptEngine;
	QScriptValue * positionX, * positionY, * circleRadius, * elapsedTime;
	QString positionScript;

	LPDIRECTDRAW pDD;
	LPDIRECTDRAWSURFACE pDDSPrimary;
	LPDIRECTDRAWSURFACE pDDSBack;
	BOOL bActive;

	QTime sceneTime;

	QTimer * timer;
};

#endif