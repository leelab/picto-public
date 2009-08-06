#include <QNetworkInterface>

#include "phidgets.h"

Phidgets::Phidgets() :
	textLCDSerialNumber(0),
	rotaryTouchSerialNumber(0),
	encoderSerialNumber(0),
	interfaceKitSerialNumber(0),
	rewardDuration(40),
	trialNumber(0),
	previousValue(-1),
	hTextLCD(0),
	hRotaryTouch(0),
	hEncoder(0),
	hManager(0)
{
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(drawLCD()));
	timer->start(100);

	systemName = "PictoBox 408"; //this would be read from the configuration file
	rewardDuration = 40;         //this would be read from the configuration file

	systemStatus = SystemStatus::disconnected;

	CPhidgetManager_create(&hManager);

	CPhidgetManager_set_OnAttach_Handler(hManager, ManagerAttachHandler, this);
	CPhidgetManager_set_OnDetach_Handler(hManager, ManagerDetachHandler, this);
	CPhidgetManager_set_OnError_Handler(hManager, ManagerErrorHandler, this);

	CPhidgetManager_open(hManager);

	int numDevices;
	CPhidgetHandle *devices;
	
	do
	{
		CPhidgetManager_getAttachedDevices(hManager, &devices, &numDevices);
		CPhidgetManager_freeAttachedDevicesArray(devices);
	} while (numDevices != 3);

	int result;
	const char *err;

	CPhidgetTextLCD_create(&hTextLCD);

	CPhidget_set_OnAttach_Handler((CPhidgetHandle) hTextLCD, TextLCDAttachHandler, this);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle) hTextLCD, TextLCDDetachHandler, this);
	CPhidget_set_OnError_Handler((CPhidgetHandle) hTextLCD, TextLCDErrorHandler, this);

	CPhidget_open((CPhidgetHandle) hTextLCD, textLCDSerialNumber);

	if((result = CPhidget_waitForAttachment((CPhidgetHandle) hTextLCD, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		return;
	}

	CPhidgetTextLCD_setCustomCharacter(hTextLCD, 8, 476859, 461256); //milliseconds character

	updateTextLCD();
	drawLCD();
	CPhidgetTextLCD_setContrast(hTextLCD, 128);
/*
	CPhidgetInterfaceKit_create(&hRotaryTouch);

	CPhidget_set_OnAttach_Handler((CPhidgetHandle)hRotaryTouch, RotaryTouchAttachHandler, this);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)hRotaryTouch, RotaryTouchDetachHandler, this);
	CPhidget_set_OnError_Handler((CPhidgetHandle)hRotaryTouch, RotaryTouchErrorHandler, this);

	CPhidgetInterfaceKit_set_OnInputChange_Handler(hRotaryTouch, RotaryTouchInputChangeHandler, this);
	CPhidgetInterfaceKit_set_OnSensorChange_Handler(hRotaryTouch, RotaryTouchSensorChangeHandler, this);

	CPhidget_open((CPhidgetHandle) hRotaryTouch, rotaryTouchSerialNumber);

	if((result = CPhidget_waitForAttachment((CPhidgetHandle) hRotaryTouch, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		return;
	}

	int numSensors = 0;

	CPhidgetInterfaceKit_getSensorCount(hRotaryTouch, &numSensors);

	for(int i = 0; i < numSensors; i++)
	{
		CPhidgetInterfaceKit_setSensorChangeTrigger(hRotaryTouch, i, 10);
	}

	CPhidgetInterfaceKit_setRatiometric(hRotaryTouch, PFALSE);
*/

	CPhidgetEncoder_create(&hEncoder);

	CPhidget_set_OnAttach_Handler((CPhidgetHandle) hEncoder, EncoderAttachHandler, this);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle) hEncoder, EncoderDetachHandler, this);
	CPhidget_set_OnError_Handler((CPhidgetHandle) hEncoder, EncoderErrorHandler, this);

	CPhidgetEncoder_set_OnInputChange_Handler(hEncoder, EncoderInputChangeHandler, this);

	CPhidgetEncoder_set_OnPositionChange_Handler(hEncoder, EncoderPositionChangeHandler, this);

	CPhidget_open((CPhidgetHandle) hEncoder, encoderSerialNumber);

	if((result = CPhidget_waitForAttachment((CPhidgetHandle) hEncoder, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		return;
	}
}

Phidgets::~Phidgets()
{
	CPhidgetTextLCD_setDisplayString(hTextLCD, 0, "");
	CPhidgetTextLCD_setDisplayString(hTextLCD, 1, "");

//	CPhidget_close((CPhidgetHandle) hRotaryTouch);
//	CPhidget_delete((CPhidgetHandle) hRotaryTouch);
	CPhidget_close((CPhidgetHandle) hEncoder);
	CPhidget_delete((CPhidgetHandle) hEncoder);

	CPhidget_close((CPhidgetHandle) hTextLCD);
	CPhidget_delete((CPhidgetHandle) hTextLCD);

	CPhidgetManager_close(hManager);
	CPhidgetManager_delete(hManager);
}

int __stdcall TextLCDAttachHandler(CPhidgetHandle hTextLCD, void *)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName (hTextLCD, &name);
	CPhidget_getSerialNumber(hTextLCD, &serialNo);

	return 0;
}

int __stdcall TextLCDDetachHandler(CPhidgetHandle hTextLCD, void * phidgetsObject)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;

	int serialNo;
	const char *name;

	CPhidget_getDeviceName (hTextLCD, &name);
	CPhidget_getSerialNumber(hTextLCD, &serialNo);

	if(phidgets->textLCDSerialNumber == serialNo) phidgets->textLCDSerialNumber = 0;

	return 0;
}

int __stdcall TextLCDErrorHandler(CPhidgetHandle, void *, int, const char *)
{
	return 0;
}

int __stdcall RotaryTouchAttachHandler(CPhidgetHandle hRotaryTouch, void *)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName(hRotaryTouch, &name);
	CPhidget_getSerialNumber(hRotaryTouch, &serialNo);

	return 0;
}

int __stdcall RotaryTouchDetachHandler(CPhidgetHandle hRotaryTouch, void * phidgetsObject)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;
	int serialNo;
	const char *name;

	CPhidget_getDeviceName (hRotaryTouch, &name);
	CPhidget_getSerialNumber(hRotaryTouch, &serialNo);

	if(phidgets->rotaryTouchSerialNumber == serialNo) phidgets->rotaryTouchSerialNumber = 0;

	return 0;
}

int __stdcall RotaryTouchErrorHandler(CPhidgetHandle, void *, int, const char *)
{
	return 0;
}

int __stdcall RotaryTouchInputChangeHandler(CPhidgetInterfaceKitHandle, void * phidgetsObject, int index, int state)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;

	if(index==1) //hover
	{
		if(state==1) //hovering
		{
			//if(phidgets->hTextLCD && phidgets->textLCDSerialNumber) CPhidgetTextLCD_setBacklight(phidgets->hTextLCD, PTRUE);
		}
		else //removed
		{
			//if(phidgets->hTextLCD && phidgets->textLCDSerialNumber) CPhidgetTextLCD_setBacklight(phidgets->hTextLCD, PFALSE);
		}
	}
	else //touch
	{
		if(state==1) //touching
		{
			if(phidgets->hTextLCD && phidgets->textLCDSerialNumber) CPhidgetTextLCD_setBacklight(phidgets->hTextLCD, PTRUE);
		}
		else //removed
		{
			phidgets->previousValue = -1;
			if(phidgets->hTextLCD && phidgets->textLCDSerialNumber) CPhidgetTextLCD_setBacklight(phidgets->hTextLCD, PFALSE);
		}
	}

	return 0;
}

int __stdcall RotaryTouchSensorChangeHandler(CPhidgetInterfaceKitHandle, void * phidgetsObject, int, int value)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;

	double updateRate = 0.1;

	if(phidgets->previousValue != -1 && value != phidgets->previousValue)
	{
		if(value - phidgets->previousValue > 500) phidgets->previousValue += 1000;
		else if(phidgets->previousValue - value > 500)phidgets-> previousValue -= 1000;

		phidgets->rewardDuration += (int) (((double) (value - phidgets->previousValue)) * updateRate);
		if(phidgets->rewardDuration < 1) phidgets->rewardDuration = 1;
		else if(phidgets->rewardDuration > 999) phidgets->rewardDuration = 999;
	}

	phidgets->previousValue = value;

	phidgets->updateTextLCD();

	return 0;
}

int __stdcall EncoderAttachHandler(CPhidgetHandle hEncoder, void *)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName(hEncoder, &name);
	CPhidget_getSerialNumber(hEncoder, &serialNo);

	return 0;
}

int __stdcall EncoderDetachHandler(CPhidgetHandle hEncoder, void * phidgetsObject)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;
	int serialNo;
	const char *name;

	CPhidget_getDeviceName (hEncoder, &name);
	CPhidget_getSerialNumber(hEncoder, &serialNo);

	if(phidgets->encoderSerialNumber == serialNo) phidgets->encoderSerialNumber = 0;

	return 0;
}

int __stdcall EncoderErrorHandler(CPhidgetHandle, void *, int, const char *)
{
	return 0;
}

int __stdcall EncoderInputChangeHandler(CPhidgetEncoderHandle,
                                        void * phidgetsObject,
                                        int, //index
                                        int state)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;

	if(state==1) //pressed
	{
		if(phidgets->hTextLCD && phidgets->textLCDSerialNumber) CPhidgetTextLCD_setBacklight(phidgets->hTextLCD, PFALSE);
	}
	else //normal
	{
		phidgets->previousValue = -1;
		if(phidgets->hTextLCD && phidgets->textLCDSerialNumber) CPhidgetTextLCD_setBacklight(phidgets->hTextLCD, PTRUE);
	}

	return 0;
}

int __stdcall EncoderPositionChangeHandler(CPhidgetEncoderHandle, //hEncoder
                                           void * phidgetsObject,
                                           int, //index
                                           int, //time
                                           int relativePosition)
{
	Phidgets * phidgets = (Phidgets *) phidgetsObject;

	double updateRate = -1.0;

	phidgets->rewardDuration += (int) (((double) relativePosition) * updateRate);
	if(phidgets->rewardDuration < 1) phidgets->rewardDuration = 1;
	else if(phidgets->rewardDuration > 999) phidgets->rewardDuration = 999;

	phidgets->updateTextLCD();

	return 0;
}
//////////////////////////////////////
int __stdcall ManagerAttachHandler(CPhidgetHandle phid, void * phidgetsObject)
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
}

void Phidgets::drawLCD()
{
	CPhidgetTextLCD_setDisplayString(hTextLCD, 0, headerRow.toLocal8Bit().data());
	CPhidgetTextLCD_setDisplayString(hTextLCD, 1, dataRow.toLocal8Bit().data());
}

void Phidgets::updateTextLCD()
{
	if(!hTextLCD || !textLCDSerialNumber)
	{
		return;
	}

	headerRow = systemName;
	headerRow += QString(20 - headerRow.length(), ' ');
	headerRow.replace(15,4,"Rew:");

	switch(systemStatus)
	{
		case SystemStatus::connected:
		{
			dataRow = "Connected";

			break;
		}
		case SystemStatus::running:
		{
			dataRow = QString("Trial: %d").arg(trialNumber);

			break;
		}
		case SystemStatus::disconnected:
		default:
		{
			QList<QHostAddress> hostAddresses = QNetworkInterface::allAddresses();

			if(!hostAddresses.empty())
			{
				dataRow = hostAddresses[0].toString();
			}

			break;
		}
	}

	dataRow += QString(20 - dataRow.length(), ' ');
	dataRow.replace(16,3,QString("%1").arg(rewardDuration,3));
	dataRow.replace(19,2,"\x08\0");
}
