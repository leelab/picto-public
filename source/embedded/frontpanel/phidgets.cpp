#include <QNetworkInterface>
#include <QTextStream>

#include <qDebug>

#include <iostream>
#include "phidgets.h"

#define CUMULATIVETICKSTHRESH 5
#define TICKSPERSECTHRESH 150

Phidgets::Phidgets(FrontPanelInfo *panelInfo) :
	textLCDSerialNumber(0),
	encoderSerialNumber(0),
	hTextLCD(0),
	hEncoder(0),
	prevClickState(0),
	panelInfo(panelInfo),
	cumRot(0),
	cumTime(0),
	turnWasTriggered_(false)
{
	QTextStream outstream(stdout);

	connect(this,SIGNAL(userInputSignal(int)),this,SLOT(turnWasTriggered(int)));
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
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 8, 214715, 395400); //milliseconds character
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 9, 506248, 8590); //arrow character
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 10, 31, 0); //horizontal line across top of matrix (used for underlining)
	//CPhidgetTextLCD_setCustomCharacter(hTextLCD, 11, 330,558); //frown face
	//CPhidgetTextLCD_setCustomCharacter(hTextLCD, 12, 330,31); //neutral face
	//CPhidgetTextLCD_setCustomCharacter(hTextLCD, 13, 557386,14); //smile face
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 11, 338926,0); //disconnected
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 12, 338926,490496); //idle
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 13, 1030276,135647); //stopped
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 14, 766150,149828); //running
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 15, 1020102,338244); //paused


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

	//Input Signal Inteface Kit
	CPhidgetInterfaceKit_create(&hIntKit);
	CPhidget_set_OnAttach_Handler((CPhidgetHandle) hIntKit, InterfaceKitAttachHandler, this);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle) hIntKit, InterfaceKitDetachHandler, this);
	CPhidget_set_OnError_Handler((CPhidgetHandle) hIntKit, InterfaceKitErrorHandler, this);
	CPhidgetInterfaceKit_set_OnInputChange_Handler(hIntKit,InterfaceKitInputChangeHandler,this);
	CPhidget_open((CPhidgetHandle) hIntKit, -1);
}

Phidgets::~Phidgets()
{
	CPhidgetTextLCD_setDisplayString(hTextLCD, 0, "");
	CPhidgetTextLCD_setDisplayString(hTextLCD, 1, "");

	CPhidget_close((CPhidgetHandle) hEncoder);
	CPhidget_delete((CPhidgetHandle) hEncoder);

	CPhidget_close((CPhidgetHandle) hTextLCD);
	CPhidget_delete((CPhidgetHandle) hTextLCD);
}



void Phidgets::updateLCD(int line, QString text)
{
	QTextStream outstream(stdout);

	if(!hTextLCD || !textLCDSerialNumber)
	{
		outstream<<"Missing handle or serial number";
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
		CPhidgetTextLCD_setDisplayString(hTextLCD, 0, LCDRow1.toLocal8Bit().data());
	}
	else if(line == 2)
	{
		LCDRow2 = text;
		CPhidgetTextLCD_setDisplayString(hTextLCD, 1, LCDRow2.toLocal8Bit().data());
	}
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

void Phidgets::dialTurnedLeft(bool fast)
{
	//This function is called from the phidgets thread.  The qt system only 
	//actually triggers the emitted signals below once control returns to the
	//qt thread.  We want to avoid having multiple userInputSignals for 
	//turn events queued up so we set a turnWasTriggered bool before emitting the userInputSignal
	//and unset it in a slot connected to that signal.  Each time this function
	//is called, we just check if the bool, and if it's true we 
	//don't emit a new turn signal.
	bool doit = true;
	mutex_.lock();
	if(turnWasTriggered_)
		doit = false;
	turnWasTriggered_ = true;
	mutex_.unlock();
	if(!doit)
		return;
	if(fast)
		emit userInputSignal(PanelInfo::rotateLeftFast);
	else
		emit userInputSignal(PanelInfo::rotateLeft);
}

void Phidgets::dialTurnedRight(bool fast)
{
	bool doit = true;
	mutex_.lock();
	if(turnWasTriggered_)
		doit = false;
	turnWasTriggered_ = true;
	mutex_.unlock();
	if(!doit)
		return;
	if(fast)
		emit userInputSignal(PanelInfo::rotateRightFast);
	else
		emit userInputSignal(PanelInfo::rotateRight);
}

void Phidgets::externalButtonClick(int index)
{
	switch(index)
	{
	case 0:
		emit userInputSignal(PanelInfo::rewardButton);
		break;
	case 7:
		emit userInputSignal(PanelInfo::flushButton);
		break;
	}
}

//This gets called whenever a userInputSignal is triggered by the qt event loop.
//By using the turnWasTriggered_ bool, we can be sure that we don't 
//put multiple turn signals into the qt event queue before earlier ones were 
//triggered.
void Phidgets::turnWasTriggered(int)
{
	mutex_.lock();
	turnWasTriggered_ = false;
	mutex_.unlock();
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
// Interface Kit
//---------------------------------------------------

int __stdcall InterfaceKitAttachHandler(CPhidgetHandle hIntKit, void * phidgetsObj)
{
	return 0;
}

int __stdcall InterfaceKitDetachHandler(CPhidgetHandle hIntKit, void * phidgetsObj)
{
	return 0;
}

int __stdcall InterfaceKitErrorHandler(CPhidgetHandle hIntKit, void * phidgetsObj, int ErrorCode, const char * Description)
{
	return 0;
}

int __stdcall InterfaceKitInputChangeHandler(CPhidgetInterfaceKitHandle hIntKit, void * phidgetsObj, int index, int state)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObj;
	if(state)
		phidgets->externalButtonClick(index);
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
	phidgets->cumTime = 0;

	return 0;
}

int __stdcall EncoderDetachHandler(CPhidgetHandle, void * phidgetsObject)
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
//We'll instead call a function within the phidgets object to emit a signal
int __stdcall EncoderInputChangeHandler(CPhidgetEncoderHandle,
                                        void * phidgetsObject,
                                        int, //index
                                        int state)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;

	if(state==0 && phidgets->prevClickState==1) //released
		phidgets->buttonClicked();

	phidgets->prevClickState = state;

	//kill the rotation counter and cumulative time on any button clicking
	phidgets->cumRot = 0;
	phidgets->cumTime = 0;

	return 0;
}

int __stdcall EncoderPositionChangeHandler(CPhidgetEncoderHandle, //hEncoder
                                           void * phidgetsObject,
                                           int, //index
                                           int time,
                                           int relativePosition)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;

	//Somtimes the rotary phidget sends faulty data, ie. The user will be 
	//slowly turning to the right and suddenly a small left turn signal
	//will come in.  To get rid of this type of error, we look at 
	//cumulative rotation and don't trigger any events unless the
	//cumulative rotation is above a CUMULATIVETICKSTHRESH value.

	//keep track of cumulative rotation and the time it took to 
	//achieve that rotation, resetting in the event of a direction change
	if(phidgets->lastRot * relativePosition < 0)
	{
		phidgets->cumRot = relativePosition;
		phidgets->cumTime = time;
	}
	else
	{
		phidgets->cumRot += relativePosition;
		phidgets->cumTime += time;
	}
	phidgets->lastRot = relativePosition;
	double ticksPerSec = phidgets->cumRot/(phidgets->cumTime*.001);

	//check for CUMULATIVETICKSTHRESH ticks
	if(abs(phidgets->cumRot) > CUMULATIVETICKSTHRESH)
	{
		//Now that we're here, we're going to send information.  If
		//the cumulative ticks that were received occured over a short
		//time, trigger a fast turn, otherwise, trigger a slow turn.
		if(ticksPerSec > 0)
		{
			phidgets->dialTurnedLeft(ticksPerSec > TICKSPERSECTHRESH);
		}
		else
		{
			phidgets->dialTurnedRight(ticksPerSec <-TICKSPERSECTHRESH);
		}
		//Now that we've triggered a turn, reset the cumulative rotation
		//and time information
		phidgets->cumRot = 0;
		phidgets->cumTime = 0;
	}
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
