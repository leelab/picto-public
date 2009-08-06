#ifndef PHIDGETS_H
#define PHIDGETS_H

#include <QTimer>

#include "phidget21.h"

namespace SystemStatus
{
	enum SystemStatus
	{
		disconnected, connected, running
	};
}

int __stdcall TextLCDAttachHandler(CPhidgetHandle hTextLCD, void * phidgetsObj);
int __stdcall TextLCDDetachHandler(CPhidgetHandle hTextLCD, void * phidgetsObj);
int __stdcall TextLCDErrorHandler(CPhidgetHandle hTextLCD, void * phidgetsObj, int ErrorCode, const char * Description);
int __stdcall RotaryTouchAttachHandler(CPhidgetHandle hRotaryTouch, void * phidgetsObj);
int __stdcall RotaryTouchDetachHandler(CPhidgetHandle hRotaryTouch, void * phidgetsObj);
int __stdcall RotaryTouchErrorHandler(CPhidgetHandle hRotaryTouch, void * phidgetsObj, int ErrorCode, const char * Description);
int __stdcall RotaryTouchInputChangeHandler(CPhidgetInterfaceKitHandle hRotaryTouch, void * phidgetsObj, int index, int state);
int __stdcall RotaryTouchSensorChangeHandler(CPhidgetInterfaceKitHandle hRotaryTouch, void * phidgetsObj, int index, int value);
int __stdcall EncoderAttachHandler(CPhidgetHandle hEncoder, void * phidgetsObj);
int __stdcall EncoderDetachHandler(CPhidgetHandle hEncoder, void * phidgetsObj);
int __stdcall EncoderErrorHandler(CPhidgetHandle hEncoder, void * phidgetsObj, int ErrorCode, const char * Description);
int __stdcall EncoderInputChangeHandler(CPhidgetEncoderHandle hEncoder, void * phidgetsObj, int index, int state);
int __stdcall EncoderPositionChangeHandler(CPhidgetEncoderHandle hEncoder, void * phidgetsObj, int index, int time, int relativePosition);
int __stdcall ManagerAttachHandler(CPhidgetHandle phid, void * phidgetsObj);
int __stdcall ManagerDetachHandler(CPhidgetHandle phid, void * phidgetsObj);
int __stdcall ManagerErrorHandler(CPhidgetManagerHandle hManager, void * phidgetsObj, int Code, const char * Description);

class Phidgets : QObject
{
	Q_OBJECT

public slots:

	void drawLCD();

public:
	Phidgets();
	~Phidgets();

	void updateTextLCD();

	int textLCDSerialNumber;
	int rotaryTouchSerialNumber;
	int encoderSerialNumber;
	int interfaceKitSerialNumber;
	int rewardDuration;
	int trialNumber;
	int previousValue;
	
	QString systemName;

	QString headerRow;
	QString dataRow;

	QTimer * timer;

	SystemStatus::SystemStatus systemStatus;

	CPhidgetTextLCDHandle hTextLCD;
	CPhidgetInterfaceKitHandle hRotaryTouch;
	CPhidgetManagerHandle hManager;
	CPhidgetEncoderHandle hEncoder;
};

#endif