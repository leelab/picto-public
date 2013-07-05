#ifndef PHIDGETS_H
#define PHIDGETS_H

#include <QTimer>
#include <QMutex>

#include "phidget21.h"
#include "FrontPanelInfo.h"


int __stdcall TextLCDAttachHandler(CPhidgetHandle hTextLCD, void * phidgetsObj);
int __stdcall TextLCDDetachHandler(CPhidgetHandle hTextLCD, void * phidgetsObj);
int __stdcall TextLCDErrorHandler(CPhidgetHandle hTextLCD, void * phidgetsObj, int ErrorCode, const char * Description);
int __stdcall InterfaceKitAttachHandler(CPhidgetHandle hIntKit, void * phidgetsObj);
int __stdcall InterfaceKitDetachHandler(CPhidgetHandle hIntKit, void * phidgetsObj);
int __stdcall InterfaceKitErrorHandler(CPhidgetHandle hIntKit, void * phidgetsObj, int ErrorCode, const char * Description);
int __stdcall InterfaceKitInputChangeHandler(CPhidgetInterfaceKitHandle hIntKit, void * phidgetsObj, int index, int state);
int __stdcall EncoderAttachHandler(CPhidgetHandle hEncoder, void * phidgetsObj);
int __stdcall EncoderDetachHandler(CPhidgetHandle hEncoder, void * phidgetsObj);
int __stdcall EncoderErrorHandler(CPhidgetHandle hEncoder, void * phidgetsObj, int ErrorCode, const char * Description);
int __stdcall EncoderInputChangeHandler(CPhidgetEncoderHandle hEncoder, void * phidgetsObj, int index, int state);
int __stdcall EncoderPositionChangeHandler(CPhidgetEncoderHandle hEncoder, void * phidgetsObj, int index, int time, int relativePosition);
//int __stdcall ManagerAttachHandler(CPhidgetHandle phid, void * phidgetsObj);
//int __stdcall ManagerDetachHandler(CPhidgetHandle phid, void * phidgetsObj);
//int __stdcall ManagerErrorHandler(CPhidgetManagerHandle hManager, void * phidgetsObj, int Code, const char * Description);

/*! \brief Interface to the Phidgets devices
 *
 *	The front panel deals with two different Phidgets devices: a 2-line LCD (1203)
 *	and a rotary encoder with push button (1052).  This object/collection of functions
 *	handles all of the I/O for these devices.
 */

class Phidgets : public QObject
{
	Q_OBJECT

public slots:
	void updateLCD(int line, QString text);
	void toggleBacklight();
	void turnOnBacklight();
	void turnOffBacklight();

signals:
	void userInputSignal(int type);


public:
	Phidgets(FrontPanelInfo *panelInfo);
	virtual ~Phidgets();

	void buttonClicked();
	void dialTurnedLeft(bool fast);
	void dialTurnedRight(bool fast);
	void externalButtonClick(int index);

	FrontPanelInfo *panelInfo;

	int textLCDSerialNumber;
	int encoderSerialNumber;
	
	QString LCDRow1;
	QString LCDRow2;

	CPhidgetTextLCDHandle hTextLCD;
	CPhidgetInterfaceKitHandle hIntKit;
	//CPhidgetManagerHandle hManager;
	CPhidgetEncoderHandle hEncoder;

	//rotaryencoder stuff
	int lastRot;
	int cumRot;
	int cumTime;
	int prevClickState;
private:
	QMutex mutex_;
	bool turnWasTriggered_;
private slots:
	void turnWasTriggered(int);
};

#endif