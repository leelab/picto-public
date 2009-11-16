#ifndef PHIDGETS_H
#define PHIDGETS_H

#include <QTimer>

#include "phidget21.h"
#include "FrontPanelInfo.h"


int __stdcall TextLCDAttachHandler(CPhidgetHandle hTextLCD, void * phidgetsObj);
int __stdcall TextLCDDetachHandler(CPhidgetHandle hTextLCD, void * phidgetsObj);
int __stdcall TextLCDErrorHandler(CPhidgetHandle hTextLCD, void * phidgetsObj, int ErrorCode, const char * Description);
int __stdcall EncoderAttachHandler(CPhidgetHandle hEncoder, void * phidgetsObj);
int __stdcall EncoderDetachHandler(CPhidgetHandle hEncoder, void * phidgetsObj);
int __stdcall EncoderErrorHandler(CPhidgetHandle hEncoder, void * phidgetsObj, int ErrorCode, const char * Description);
int __stdcall EncoderInputChangeHandler(CPhidgetEncoderHandle hEncoder, void * phidgetsObj, int index, int state);
int __stdcall EncoderPositionChangeHandler(CPhidgetEncoderHandle hEncoder, void * phidgetsObj, int index, int time, int relativePosition);
//int __stdcall ManagerAttachHandler(CPhidgetHandle phid, void * phidgetsObj);
//int __stdcall ManagerDetachHandler(CPhidgetHandle phid, void * phidgetsObj);
//int __stdcall ManagerErrorHandler(CPhidgetManagerHandle hManager, void * phidgetsObj, int Code, const char * Description);

class Phidgets : public QObject
{
	Q_OBJECT

public slots:
	void updateLCD(int line, QString text);
	void toggleBacklight();
	void turnOnBacklight();

signals:
	void userInputSignal(int type);


public:
	Phidgets(FrontPanelInfo *panelInfo);
	~Phidgets();

	void buttonClicked();
	void dialTurnedLeft();
	void dialTurnedRight();

	FrontPanelInfo *panelInfo;

	int textLCDSerialNumber;
	int encoderSerialNumber;
	
	QString LCDRow1;
	QString LCDRow2;

	CPhidgetTextLCDHandle hTextLCD;
	//CPhidgetManagerHandle hManager;
	CPhidgetEncoderHandle hEncoder;

	//rotaryencoder stuff
	int lastRot;
	int cumRot;
	int prevClickState;
};

#endif