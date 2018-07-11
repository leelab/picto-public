#ifndef PHIDGETS_H
#define PHIDGETS_H

#include <QTimer>
#include <QMutex>


#include "FrontPanelInfo.h"


/*! \brief Provides an interface to the Phidgets devices used by the FrontPanel Application
 * \details The front panel deals with two different Phidgets devices: a 2-line LCD (1203)
 * and a rotary encoder with push button (1052).  This object
 * handles all of the I/O for these devices.  It does this by connecting to the Phidgets device API.
 * This file includes a number of global callback functions that the Phidgets API calls whenever user 
 * input events occur.  Internally, these functions call the Phidgets object which in turn sends messages
 * to the FrontPanel application.  It should be noted that some threading issues had to be considered
 * here since calls from the Phidgets API run in a seperate thread from the one used in the Front
 * Panel application.
 * \author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2015
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
	/*! \brief Emitted whenever an event of type PanelInfo::InputType occurs on the phidgets interface
	*/
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