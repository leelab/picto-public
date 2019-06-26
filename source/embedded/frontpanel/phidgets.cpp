#include <QNetworkInterface>
#include <QTextStream>

#include <qDebug>
#include <QFile>

#include <iostream>
#include "phidgets.h"

#define CUMULATIVETICKSTHRESH 5
#define TICKSPERSECTHRESH 150

#include "phidget22.h"
#include <qfile.h>
//CPhidgetTextLCDHandle hTextLCD;
PhidgetLCDHandle  hTextLCD;

//CPhidgetInterfaceKitHandle hIntKit;
PhidgetDigitalInputHandle hDigIn0;
PhidgetDigitalInputHandle hDigIn7;
PhidgetDigitalInputHandle hDigInDial;

//CPhidgetManagerHandle hManager;
PhidgetEncoderHandle hEncoder;

//These functions are used to interface with the Phidgets API
static void CCONV TextLCDAttachHandler(PhidgetHandle hTextLCD, void * phidgetsObj);
static void CCONV TextLCDDetachHandler(PhidgetHandle hTextLCD, void * phidgetsObj);
static void CCONV TextLCDErrorHandler(PhidgetHandle hTextLCD, void * phidgetsObj, Phidget_ErrorEventCode ErrorCode, const char * Description);
static void CCONV EncoderAttachHandler(PhidgetHandle hEncoder, void * phidgetsObj);
static void CCONV EncoderDetachHandler(PhidgetHandle hEncoder, void * phidgetsObj);
static void CCONV EncoderErrorHandler(PhidgetHandle hEncoder, void * phidgetsObj, Phidget_ErrorEventCode ErrorCode, const char * Description);
static void CCONV EncoderPositionChangeHandler(PhidgetEncoderHandle hEncoder, void * phidgetsObj, int relativePosition, double time, int indexTriggered);
//int __stdcall ManagerAttachHandler(CPhidgetHandle phid, void * phidgetsObj);
//int __stdcall ManagerDetachHandler(CPhidgetHandle phid, void * phidgetsObj);
//int __stdcall ManagerErrorHandler(CPhidgetManagerHandle hManager, void * phidgetsObj, int Code, const char * Description);

//2 dig input on the LCD device
static void CCONV DigitalInput0ChangeHandler(PhidgetDigitalInputHandle hDigIn0, void * phidgetsObject, int state);
static void CCONV DigitalInput7ChangeHandler(PhidgetDigitalInputHandle hDigIn7, void * phidgetsObject, int state);

//press encoder rotary dial as a button: hDigInDial
static void CCONV EncoderInputChangeHandler(PhidgetDigitalInputHandle hEncoder, void * phidgetsObj, int state);

/*! \brief Constructs a new Phidgets object, initializes its data values and sets up its interaction with the Phidgets API
 * 
 */
Phidgets::Phidgets(FrontPanelInfo *panelInfo) :
	textLCDSerialNumber(0),
	encoderSerialNumber(0),
	prevClickState(0),
	panelInfo(panelInfo),
	cumRot(0),
	cumTime(0),
	turnWasTriggered_(false)
{
	QTextStream outstream(stdout);

	/*QString filename = "C://Picto//bin//Phidgets.txt";
	QFile file(filename);
	if (file.open(QIODevice::ReadWrite))
	{
		QTextStream stream(&file);
		stream << "Phidgets" << endl;
		stream.flush();
	}*/

	connect(this, SIGNAL(userInputSignal(int)), this, SLOT(turnWasTriggered(int)));
	int result;
	const char *err;

	//Text LCD
	//--------
	PhidgetReturnCode res;
	res = PhidgetLCD_create(&hTextLCD);
	if (res != EPHIDGET_OK) {
		fprintf(stderr, "failed to create LCD channel\n");
		exit(1);
	}
	Phidget_setOnAttachHandler((PhidgetHandle)hTextLCD, TextLCDAttachHandler, this);
	Phidget_setOnDetachHandler((PhidgetHandle)hTextLCD, TextLCDDetachHandler, this);
	Phidget_setOnErrorHandler((PhidgetHandle)hTextLCD, TextLCDErrorHandler, this);
	res = Phidget_openWaitForAttachment((PhidgetHandle)hTextLCD, 10000);
	if (res != EPHIDGET_OK)
	{
		Phidget_getErrorDescription(res, &err);
		outstream << "Text LCD never attached.\n Error: " << err << "\n";
		outstream.flush();
		return;
	}	
	
	/*
	//Set up custom characters that will be displayed in the LCD
	//Custom character creator: http://www.phidgets.com/documentation/customchar.html
	//CPhidgetTextLCD_setCustomCharacter(hTextLCD, 7, 23378, 276774); //picto not running character
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 8, 214715, 395400); //milliseconds character
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 9, 506248, 8590); //arrow character
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 10, 31, 0); //horizontal line across top of matrix (used for underlining)
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 11, 338926, 0); //disconnected
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 12, 338926, 490496); //idle
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 13, 1030276, 135647); //stopped
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 14, 766150, 149828); //running
	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 15, 23378, 276774); //picto not running character
	*/

	// https://www.phidgets.com/docs/LCD_Character_Display_Primer#Text_LCDs
	//Set up custom characters that will be displayed in the LCD
	//Custom character creator: http://www.phidgets.com/documentation/customchar.html
	uint8_t sBitmap[] = {
		0x01, 0x01, 0x01, 0x01, 0x01,
		0x01, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x01, 0x01, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x01,
		0x01, 0x01, 0x00, 0x01, 0x01,
	};
	PhidgetLCD_setCharacterBitmap(hTextLCD, FONT_5x8, "S", sBitmap);

	//vertical cursor
	uint8_t arrowBitmap[] = {
		0x01, 0x00, 0x00, 0x00, 0x00,
		0x01, 0x01, 0x00, 0x00, 0x00,
		0x01, 0x01, 0x01, 0x00, 0x00,
		0x01, 0x01, 0x01, 0x01, 0x00,
		0x01, 0x01, 0x01, 0x00, 0x00,
		0x01, 0x01, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00,
	};
	PhidgetLCD_setCharacterBitmap(hTextLCD, FONT_5x8, "\x09", arrowBitmap);

	//Reward (ms)
	uint8_t msBitmap[] = {
		0x00, 0x01, 0x00, 0x01, 0x00,
		0x01, 0x00, 0x01, 0x00, 0x01,
		0x01, 0x00, 0x00, 0x00, 0x01,
		0x00, 0x01, 0x01, 0x01, 0x01,
		0x01, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x01, 0x01, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01,
	};
	PhidgetLCD_setCharacterBitmap(hTextLCD, FONT_5x8, "\x8", msBitmap);

	//cursor underlining letter when changing name
	uint8_t xABitmap[] = {
		0x00, 0x00, 0x01, 0x00, 0x00,
		0x00, 0x01, 0x01, 0x01, 0x00,
		0x01, 0x01, 0x01, 0x01, 0x01,
		0x00, 0x01, 0x01, 0x01, 0x00,
		0x00, 0x01, 0x01, 0x01, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00,
	};
	PhidgetLCD_setCharacterBitmap(hTextLCD, FONT_5x8, "\xA", xABitmap);

	//default status
	uint8_t xFBitmap[] = {
		0x00, 0x00, 0x01, 0x00, 0x00,
		0x00, 0x01, 0x01, 0x01, 0x00,
		0x01, 0x01, 0x01, 0x01, 0x01,
		0x00, 0x01, 0x01, 0x01, 0x00,
		0x00, 0x01, 0x01, 0x01, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x01, 0x01, 0x01, 0x00,
		0x00, 0x00, 0x01, 0x00, 0x00,
	};
	PhidgetLCD_setCharacterBitmap(hTextLCD, FONT_5x8, "\xF", xFBitmap);

	//temorary:
	PhidgetLCD_setCharacterBitmap(hTextLCD, FONT_5x8, "\xB", xFBitmap);
	PhidgetLCD_setCharacterBitmap(hTextLCD, FONT_5x8, "\xC", xFBitmap);
	PhidgetLCD_setCharacterBitmap(hTextLCD, FONT_5x8, "\xD", xFBitmap);
	PhidgetLCD_setCharacterBitmap(hTextLCD, FONT_5x8, "\xE", xFBitmap);
	

	PhidgetLCD_setContrast(hTextLCD, 128);

	//Rotary Encoder
	//--------------
	res = PhidgetEncoder_create(&hEncoder);
	if (res != EPHIDGET_OK)
	{
		fprintf(stderr, "failed to create Encoder input channel\n");
		exit(1);
	}

	Phidget_setOnAttachHandler((PhidgetHandle)hEncoder, EncoderAttachHandler, this);
	Phidget_setOnDetachHandler((PhidgetHandle)hEncoder, EncoderDetachHandler, this);
	Phidget_setOnErrorHandler((PhidgetHandle)hEncoder, EncoderErrorHandler, this);
	PhidgetEncoder_setOnPositionChangeHandler(hEncoder, EncoderPositionChangeHandler, this);

	res = Phidget_openWaitForAttachment((PhidgetHandle)hEncoder, 10000);
	if (res != EPHIDGET_OK)
	{
		Phidget_getErrorDescription(res, &err);

		outstream << "Rotary encoder never attached.\nError: " << err << "\n";
		outstream.flush();
		return;
	}

	//Digital Input of the Encoder (pressing the dial)
	PhidgetReturnCode prc;
	prc = PhidgetDigitalInput_create(&hDigInDial);
	if (EPHIDGET_OK != prc) return;

	Phidget_setDeviceSerialNumber((PhidgetHandle)hDigInDial, encoderSerialNumber);
	Phidget_setChannel((PhidgetHandle)hDigInDial, 0);

	PhidgetDigitalInput_setOnStateChangeHandler(hDigInDial, EncoderInputChangeHandler, this);

	res = Phidget_openWaitForAttachment((PhidgetHandle)hDigInDial, 10000);
	if (res != EPHIDGET_OK)
	{
		Phidget_getErrorDescription(res, &err);
		PhidgetDigitalInput_delete(&hDigInDial);
		outstream << "Digital Input Dial error.\nError: " << err << "\n";
		outstream.flush();
	}

	prc = PhidgetDigitalInput_create(&hDigIn0);
	if (EPHIDGET_OK != prc) return;

	Phidget_setDeviceSerialNumber((PhidgetHandle)hDigIn0, textLCDSerialNumber);
	Phidget_setChannel((PhidgetHandle)hDigIn0, 0);

	PhidgetDigitalInput_setOnStateChangeHandler(hDigIn0, DigitalInput0ChangeHandler, this);

	res = Phidget_openWaitForAttachment((PhidgetHandle)hDigIn0, 10000);
	if (res != EPHIDGET_OK)
	{
		Phidget_getErrorDescription(res, &err);
		PhidgetDigitalInput_delete(&hDigIn0);
		outstream << "Digital Input 0 error.\nError: " << err << "\n";
		outstream.flush();
	}

	//hIntKit = NULL;
	prc = PhidgetDigitalInput_create(&hDigIn7);
	if (EPHIDGET_OK != prc) return;

	Phidget_setDeviceSerialNumber((PhidgetHandle)hDigIn7, textLCDSerialNumber);
	Phidget_setChannel((PhidgetHandle)hDigIn7, 7);

	PhidgetDigitalInput_setOnStateChangeHandler(hDigIn7, DigitalInput7ChangeHandler, this);

	res = Phidget_openWaitForAttachment((PhidgetHandle)hDigIn7, 10000);
	if (res != EPHIDGET_OK)
	{
		Phidget_getErrorDescription(res, &err);
		PhidgetDigitalInput_delete(&hDigIn7);
		outstream << "Digital Input 7 error.\nError: " << err << "\n";
		outstream.flush();
	}

}

/*! \brief Deinitializes the Phidgets object by clearing the LCD display and closing the Phidgets API
 */
Phidgets::~Phidgets()
{
	PhidgetLCD_clear((PhidgetLCDHandle)hTextLCD);
	Phidget_close((PhidgetHandle)hEncoder);
	PhidgetEncoder_delete(&hEncoder);
	Phidget_close((PhidgetHandle)hTextLCD);
	PhidgetLCD_delete(&hTextLCD);
}


/*! \brief Sets the input text to the input line of the LCD display
 * \details Line1 is the top line of the display, Line 2 is the bottom line
 */
void Phidgets::updateLCD(int line, QString text)
{
	QTextStream outstream(stdout);

	//make the text 20 characters long...
	if (text.size() > 20)
		text.truncate(20);
	else if (text.size() < 20)
		text += QString(20 - text.size(), ' ');

	//set the values appropriately
	if (line == 1)
	{
		LCDRow1 = text;
		
		PhidgetLCD_writeText(hTextLCD, FONT_5x8, 0, 0, LCDRow1.toLocal8Bit().data());
		PhidgetLCD_flush(hTextLCD);
	}
	else if (line == 2)
	{
		LCDRow2 = text;
		
		PhidgetLCD_writeText(hTextLCD, FONT_5x8, 0, 1, LCDRow2.toLocal8Bit().data());
		PhidgetLCD_flush(hTextLCD);
	}
	return;	
}

/*! \brief Toggles the on/off state of the LCD backlight
 */
void Phidgets::toggleBacklight()
{
	double backlightState;
	PhidgetLCD_getBacklight(hTextLCD, &backlightState);
	if (backlightState == 0)
		backlightState = 1;
	else
		backlightState = 0;
	PhidgetLCD_setBacklight(hTextLCD, backlightState);

}

/*! \brief Turns the LCD backlight on
 */
void Phidgets::turnOnBacklight()
{
	PhidgetLCD_setBacklight(hTextLCD, 1);
}

/*! \brief Turns the LCD backlight off
 */
void Phidgets::turnOffBacklight()
{	
	PhidgetLCD_setBacklight(hTextLCD, 0);
}

/*! \brief Called by the phidgets API whenever the knob is pressed and emits the buttonPush userInputSignal
*/
void Phidgets::buttonClicked()
{
	emit userInputSignal(PanelInfo::buttonPush);
}

/*! \brief Called by the phidgets API whenever the knob is turned left and emits the PanelInfo::rotateLeft or PanelInfo::rotateLeftFast userInputSignals
 * \details This function is called from the phidgets thread.  The qt system only 
 * actually triggers the emitted signals below once control returns to the
 * qt thread.  We want to avoid having multiple userInputSignals for 
 * turn events queued up so we set a turnWasTriggered bool before emitting the userInputSignal
 * and unset it in a slot connected to that signal.  Each time this function
 * is called, we just check if the bool, and if it's true we 
 * don't emit a new turn signal.
*/
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

/*! \brief Called by the phidgets API whenever the knob is turned right and emits the PanelInfo::rotateRight or PanelInfo::rotateRightFast userInputSignals
 * \details See dialTurnedLeft() for more detail
 */
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

/*! \brief Called by the phidgets API whenever the reward or flush buttons are pressed and emits the PanelInfo::rewardButton or PanelInfo::flushButton userInputSignals accordingly
 * \details In the PictoBox, the reward button is connected to digital input 0 of the LCD display.  
 * The flush button is connected to digital input 7.
 */
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

/*! \brief A function used internally to assure that multiple turn userInputSignals aren't emitted in a row before the FrontPanel Application can handle them
 * \details This gets called whenever a userInputSignal is triggered by the qt event loop in the 
 * FrontPanel Application thread, whereas we find out when the phidgets knob was turned from a separate
 * phidgets thread. By using the turnWasTriggered_ bool, we can be sure that we don't 
 * put multiple turn signals into the qt event queue before earlier ones were triggered.
 * \sa dialTurnedLeft(), dialTurnedRight()
 */
void Phidgets::turnWasTriggered(int)
{
	mutex_.lock();
	turnWasTriggered_ = false;
	mutex_.unlock();
}

//---------------------------------------------------
//   LCD Text
//---------------------------------------------------
static void CCONV TextLCDAttachHandler(PhidgetHandle hTextLCD, void *phidgetsObject)
{
	Phidgets * phidgets = (Phidgets *)phidgetsObject;

	int serialNo;
	const char *name;

	Phidget_getDeviceName(hTextLCD, &name);
	Phidget_getDeviceSerialNumber(hTextLCD, &serialNo);

	phidgets->textLCDSerialNumber = serialNo;

	//CPhidgetTextLCD_setDisplayString((CPhidgetTextLCDHandle)hTextLCD, 0, Phidgets->LCDRow1.toLocal8Bit().data());
	//CPhidgetTextLCD_setDisplayString((CPhidgetTextLCDHandle)hTextLCD, 1, Phidgets->LCDRow2.toLocal8Bit().data());
	PhidgetLCD_writeText((PhidgetLCDHandle)hTextLCD, FONT_5x8, 0, 0, phidgets->LCDRow1.toLocal8Bit().data());
	PhidgetLCD_flush((PhidgetLCDHandle)hTextLCD);
	PhidgetLCD_writeText((PhidgetLCDHandle)hTextLCD, FONT_5x8, 0, 1, phidgets->LCDRow2.toLocal8Bit().data());
	PhidgetLCD_flush((PhidgetLCDHandle)hTextLCD);

	return;
}

static void CCONV  TextLCDDetachHandler(PhidgetHandle hTextLCD, void * phidgetsObject)
{
	Phidgets * phidgets = (Phidgets *)phidgetsObject;

	phidgets->textLCDSerialNumber = 0;

	return;
}

static void CCONV TextLCDErrorHandler(PhidgetHandle hTextLCD, void * phidgetsObj, Phidget_ErrorEventCode ErrorCode, const char * Description)
{
	return;
}


//---------------------------------------------------
// Digital Input 0
//---------------------------------------------------

static void CCONV DigitalInput0ChangeHandler(PhidgetDigitalInputHandle hDigIn0, void * phidgetsObject, int state)
{
	Phidgets * phidgets = (Phidgets *)phidgetsObject;
	if (state)
		phidgets->externalButtonClick(0);
	return;
}

//---------------------------------------------------
// Digital Input 7
//---------------------------------------------------

static void CCONV DigitalInput7ChangeHandler(PhidgetDigitalInputHandle hDigIn7, void * phidgetsObject, int state)
{
	Phidgets * phidgets = (Phidgets *)phidgetsObject;
	if (state)
		phidgets->externalButtonClick(7);
	return;
}


//---------------------------------------------------
// Rotary Encoder
//---------------------------------------------------
static void CCONV EncoderAttachHandler(PhidgetHandle hEncoder, void *phidgetsObject)
{
	Phidgets * phidgets = (Phidgets *)phidgetsObject;
	int serialNo;
	const char *name;

	Phidget_getDeviceName(hEncoder, &name);
	Phidget_getDeviceSerialNumber(hEncoder, &serialNo);

	phidgets->encoderSerialNumber = serialNo;
	phidgets->lastRot = 0;
	phidgets->cumRot = 0;
	phidgets->cumTime = 0;

	return;
}

static void CCONV EncoderDetachHandler(PhidgetHandle, void * phidgetsObject)
{
	Phidgets * phidgets = (Phidgets *)phidgetsObject;

	phidgets->encoderSerialNumber = 0;

	return;
}

static void CCONV EncoderErrorHandler(PhidgetHandle hEncoder, void * phidgetsObj, Phidget_ErrorEventCode ErrorCode, const char * Description)
{
	return;
}



static void CCONV EncoderPositionChangeHandler(PhidgetEncoderHandle hEncoder, void * phidgetsObj, int relativePosition, double time, int indexTriggered)
{
	Phidgets * phidgets = (Phidgets *)phidgetsObj;

	//Somtimes the rotary phidget sends faulty data, ie. The user will be 
	//slowly turning to the right and suddenly a small left turn signal
	//will come in.  To get rid of this type of error, we look at 
	//cumulative rotation and don't trigger any events unless the
	//cumulative rotation is above a CUMULATIVETICKSTHRESH value.

	//keep track of cumulative rotation and the time it took to 
	//achieve that rotation, resetting in the event of a direction change
	if (phidgets->lastRot * relativePosition < 0)
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
	double ticksPerSec = phidgets->cumRot / (phidgets->cumTime*.001);

	//check for CUMULATIVETICKSTHRESH ticks
	if (abs(phidgets->cumRot) > CUMULATIVETICKSTHRESH)
	{
		//Now that we're here, we're going to send information.  If
		//the cumulative ticks that were received occured over a short
		//time, trigger a fast turn, otherwise, trigger a slow turn.
		if (ticksPerSec > 0)
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
	return;
}

//Since these functions can't interact directly with QT signals and slots,
//We'll instead call a function within the phidgets object to emit a signal
static void CCONV EncoderInputChangeHandler(PhidgetDigitalInputHandle hEncoder, void * phidgetsObj, int state)
{
	Phidgets * phidgets = (Phidgets *)phidgetsObj;

	if (state == 0 && phidgets->prevClickState == 1) //released
		phidgets->buttonClicked();

	phidgets->prevClickState = state;

	//kill the rotation counter and cumulative time whenever a button is clicked
	phidgets->cumRot = 0;
	phidgets->cumTime = 0;

	return;
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
