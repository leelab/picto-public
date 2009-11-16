#include <QNetworkInterface>
#include <QTextStream>

#include <qDebug>

#include "phidgets.h"

Phidgets::Phidgets(FrontPanelInfo *panelInfo) :
	textLCDSerialNumber(0),
	encoderSerialNumber(0),
	hTextLCD(0),
	hEncoder(0),
	prevClickState(0),
	panelInfo(panelInfo)
{
	QTextStream outstream(stdout);

	int result;
	const char *err;

	//Text LCD
	//--------
	CPhidgetTextLCD_create(&hTextLCD);

	CPhidget_set_OnAttach_Handler((CPhidgetHandle) hTextLCD, TextLCDAttachHandler, this);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle) hTextLCD, TextLCDDetachHandler, this);
	CPhidget_set_OnError_Handler((CPhidgetHandle) hTextLCD, TextLCDErrorHandler, this);

	CPhidget_open((CPhidgetHandle) hTextLCD, -1);

	if((result = CPhidget_waitForAttachment((CPhidgetHandle) hTextLCD, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		outstream<<"Text LCD never attached.\n Error: "<<err<<"\n";
		outstream.flush();
		return;
	}

	//Custom character creator: http://www.phidgets.com/documentation/customchar.html
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 8, 476859, 461256); //milliseconds character
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 9, 506248, 8590); //arrow character
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 10, 31, 0); //horizontal line across top of matrix (used for underlining)
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 11, 330,558); //frown face
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 12, 330,31); //neutral face
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 13, 557386,14); //smile face

	CPhidgetTextLCD_setContrast(hTextLCD, 128);

	//Rotary Encoder
	//--------------
	CPhidgetEncoder_create(&hEncoder);

	CPhidget_set_OnAttach_Handler((CPhidgetHandle) hEncoder, EncoderAttachHandler, this);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle) hEncoder, EncoderDetachHandler, this);
	CPhidget_set_OnError_Handler((CPhidgetHandle) hEncoder, EncoderErrorHandler, this);

	CPhidgetEncoder_set_OnInputChange_Handler(hEncoder, EncoderInputChangeHandler, this);

	CPhidgetEncoder_set_OnPositionChange_Handler(hEncoder, EncoderPositionChangeHandler, this);

	CPhidget_open((CPhidgetHandle) hEncoder, -1);

	if((result = CPhidget_waitForAttachment((CPhidgetHandle) hEncoder, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);

		outstream<<"Rotary encoder never attached.\nError: "<<err<<"\n";
		outstream.flush();
		return;
	}
}

Phidgets::~Phidgets()
{
	CPhidgetTextLCD_setDisplayString(hTextLCD, 0, "");
	CPhidgetTextLCD_setDisplayString(hTextLCD, 1, "");

	CPhidget_close((CPhidgetHandle) hEncoder);
	CPhidget_delete((CPhidgetHandle) hEncoder);

	CPhidget_close((CPhidgetHandle) hTextLCD);
	CPhidget_delete((CPhidgetHandle) hTextLCD);

	//CPhidgetManager_close(hManager);
	//CPhidgetManager_delete(hManager);
}



void Phidgets::updateLCD(int line, QString text)
{
	QTextStream outstream(stdout);

	if(!hTextLCD || !textLCDSerialNumber)
	{
		outstream<<"Missing handle or serial number";
		outstream.flush();
		return;
	}

	//make the text 20 characters long...
	if(text.size() > 20)
		text.truncate(20);
	else if(text.size() <20)
		text += QString(20-text.size(),' ');

	//set the values appropriately
	if(line == 1)
	{
		LCDRow1 = text;
	}
	else if(line == 2)
	{
		LCDRow2 = text;
	}

	//update the LCD
	CPhidgetTextLCD_setDisplayString(hTextLCD, 0, LCDRow1.toLocal8Bit().data());
	CPhidgetTextLCD_setDisplayString(hTextLCD, 1, LCDRow2.toLocal8Bit().data());

	return;
}

void Phidgets::toggleBacklight()
{
	int backlightState;
	
	CPhidgetTextLCD_getBacklight(hTextLCD,&backlightState);
	if(backlightState == 0)
		backlightState = 1;
	else
		backlightState = 0;
	CPhidgetTextLCD_setBacklight(hTextLCD,backlightState);

}
void Phidgets::turnOnBacklight()
{
	CPhidgetTextLCD_setBacklight(hTextLCD,1);
}


void Phidgets::buttonClicked()
{
	emit userInputSignal(PanelInfo::buttonPush);
}

void Phidgets::dialTurnedLeft()
{
	emit userInputSignal(PanelInfo::rotateLeft);
}

void Phidgets::dialTurnedRight()
{
	emit userInputSignal(PanelInfo::rotateRight);
}

//---------------------------------------------------
//   LCD Text
//---------------------------------------------------

int __stdcall TextLCDAttachHandler(CPhidgetHandle hTextLCD, void *phidgetsObject)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;

	int serialNo;
	const char *name;

	CPhidget_getDeviceName (hTextLCD, &name);
	CPhidget_getSerialNumber(hTextLCD, &serialNo);

	phidgets->textLCDSerialNumber = serialNo;

	CPhidgetTextLCD_setDisplayString((CPhidgetTextLCDHandle)hTextLCD, 0, phidgets->LCDRow1.toLocal8Bit().data());
	CPhidgetTextLCD_setDisplayString((CPhidgetTextLCDHandle)hTextLCD, 1, phidgets->LCDRow2.toLocal8Bit().data());


	return 0;
}

int __stdcall TextLCDDetachHandler(CPhidgetHandle hTextLCD, void * phidgetsObject)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;

	phidgets->textLCDSerialNumber = 0;

	return 0;
}

int __stdcall TextLCDErrorHandler(CPhidgetHandle, void *, int, const char *)
{
	return 0;
}


//---------------------------------------------------
// Rotary Encoder
//---------------------------------------------------
int __stdcall EncoderAttachHandler(CPhidgetHandle hEncoder, void *phidgetsObject)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;
	int serialNo;
	const char *name;

	CPhidget_getDeviceName(hEncoder, &name);
	CPhidget_getSerialNumber(hEncoder, &serialNo);

	phidgets->encoderSerialNumber = serialNo;
	phidgets->lastRot = 0;
	phidgets->cumRot = 0;

	return 0;
}

int __stdcall EncoderDetachHandler(CPhidgetHandle hEncoder, void * phidgetsObject)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;
	
	phidgets->encoderSerialNumber = 0;

	return 0;
}

int __stdcall EncoderErrorHandler(CPhidgetHandle, void *, int, const char *)
{
	return 0;
}



//Since these functions can't interact directly with QT signals and slots,
//We'll insted call a function within the phidgets object to emit a signal
int __stdcall EncoderInputChangeHandler(CPhidgetEncoderHandle,
                                        void * phidgetsObject,
                                        int, //index
                                        int state)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;

	if(state==0 && phidgets->prevClickState==1) //released
		phidgets->buttonClicked();

	phidgets->prevClickState = state;

	//kill the rotation counter on any button clicking
	phidgets->cumRot = 0;

	return 0;
}

int __stdcall EncoderPositionChangeHandler(CPhidgetEncoderHandle, //hEncoder
                                           void * phidgetsObject,
                                           int, //index
                                           int, //time
                                           int relativePosition)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;

	//keep track of cumulative rotation, resetting in the event
	//of a direction change
	if(phidgets->lastRot * relativePosition < 0)
	{
		phidgets->cumRot = relativePosition;
	}
	else
	{
		phidgets->cumRot += relativePosition;
	}

	//check for a 1/4 turn (20 ticks)
	if(phidgets->cumRot > 10)
	{
		phidgets->cumRot = 0;
		phidgets->dialTurnedLeft();
	}
	if(phidgets->cumRot <-10)
	{
		phidgets->cumRot = 0;
		phidgets->dialTurnedRight();
	}

	phidgets->lastRot = relativePosition;


	return 0;
}


//---------------------------------------------------
// Phidgets Manager
//---------------------------------------------------

/*int __stdcall ManagerAttachHandler(CPhidgetHandle phid, void * phidgetsObject)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;

	int serialNo;
	const char *name;
	CPhidget_DeviceID id;
	CPhidget_DeviceClass cls;

	CPhidget_getDeviceName (phid, &name);
	CPhidget_getSerialNumber(phid, &serialNo);
	CPhidget_getDeviceClass(phid, &cls);
	CPhidget_getDeviceID(phid, &id);

	if(id == PHIDID_TEXTLCD_2x20_w_8_8_8) phidgets->textLCDSerialNumber = serialNo;
	else if(id == PHIDID_ROTARY_TOUCH) phidgets->rotaryTouchSerialNumber = serialNo;
	else if(id == PHIDID_ENCODER_1ENCODER_1INPUT) phidgets->encoderSerialNumber = serialNo;
	else if(id == PHIDID_INTERFACEKIT_8_8_8_w_LCD) phidgets->interfaceKitSerialNumber = serialNo;

	return 0;
}

int __stdcall ManagerDetachHandler(CPhidgetHandle phid, void * phidgetsObject)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;

	int serialNo;
	const char *name;

	CPhidget_getDeviceName (phid, &name);
	CPhidget_getSerialNumber(phid, &serialNo);

	if(phidgets->textLCDSerialNumber == serialNo) phidgets->textLCDSerialNumber = 0;
	else if(phidgets->rotaryTouchSerialNumber == serialNo) phidgets->rotaryTouchSerialNumber = 0;
	else if(phidgets->encoderSerialNumber == serialNo) phidgets->encoderSerialNumber = 0;
	else if(phidgets->interfaceKitSerialNumber == serialNo) phidgets->interfaceKitSerialNumber = 0;

	return 0;
}

int __stdcall ManagerErrorHandler(CPhidgetManagerHandle, void *, int, const char *)
{
	return 0;
}*/
