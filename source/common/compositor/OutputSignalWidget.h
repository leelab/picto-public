#ifndef _OUTPUTSIGNALWIDGET_H_
#define _OUTPUTSIGNALWIDGET_H_

#include <QWidget>
#include <QHBoxLayout>
#include <QVector>
#include <QLCDNumber>
#include "../iodevices/VirtualOutputSignalController.h"
#include "../common.h"
namespace Picto {

/*!	\brief Provides a visual display of Pictoboxes digital output signals in the workstation
 *  \details The Pictobox provides 8 BNC digital outputs as well as 8 digital outputs on the
 *	25 pin parallel port cable.  The particular pins on the parallel cable that are used for 
 *	digital output and their functionality depends on the type of system for which the box was 
 *	built and the configuration of the director.  If the director is configured to work with a 
 *	neural data acquisition system, (no -onesided flag is used when running the director), then
 *	its parallel pin wiring depends on which Neural system it will be connected to.  (Currently 
 *	either Plexon or TDT).  In that case, the pin connected to the highest digital bit is used to
 *	strobe data into the neural system, and the other 7 pins are used automatically to send event
 *	codes for timing synchronization.  If the director is not configured to work with a neural 
 *	data acquisition system (-onesided flag is used) all 8 pins are available on the parallel port
 *	and the hardware wiring will depend on the needs of the customer for whom the Pictobox was 
 *	designed.  We did this for Arnsten lab, and in their case for example, we actually ended up 
 *	wiring one of the digital outputs to two separate parallel pins.  Regardless of the physical
 *	hardware setup, as far as the Picto experiment is concerned, the PAR0 port has 8 pins that 
 *	can be controlled as part of the experiment.  When these or the BNC0 port pins are used (ie.
 *	if a DigitalOutput or BinaryDataOutput object are used in the experiment, this
 *	OutputSignalWidget shows the current state of the output pins for the purpose of debugging
 *	as well as monitoring the experimental state of the Pictobox without using a state machine.
 *	The widget itself consists of a row of digital single digit displays, each indicating the
 *	voltage on an individual pins of the port.  The widget is controlled by the VirtualOutputSignalController
 *	passed into the constructor.
 *	\sa OutputSignalController, VirtualOutputSignalController
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API OutputSignalWidget :  public QWidget
#else
class OutputSignalWidget : public QWidget
#endif
{
	Q_OBJECT
public:
	OutputSignalWidget(QSharedPointer<Picto::VirtualOutputSignalController> outSigController);
	virtual ~OutputSignalWidget();
	void enable(bool enable);

private:
	QSharedPointer<Picto::VirtualOutputSignalController> outSigController_;
	QHBoxLayout* layout_;
	QVector<QLCDNumber*> widgets_;
	bool enable_;

private slots:
	void outputSignalChanged(int pinId,bool enabled,bool high);
};


}; //namespace Picto

#endif
