#ifndef PICTODEVICSIMULATOR_H
#define PICTODEVICSIMULATOR_H

#include <QObject>
#include <QSharedPointer>
#include <QTcpSocket>
#include <QHostAddress>
#include "PictoSystemState.h"
#include "PictoDeviceSimulatorTypes.h"
#include <QThread>
namespace PictoSim{
/*!	\brief The parent class for all device simulator objects used in the PictoTestBench
 *
 *	Device Simulators are used to simulate devices during a Picto test.  They 
 *	take in commands that define the types of messages they should send to other Picto 
 *	devices and check the responses from those devices for correctness.  Errors are 
 *	reported through the standard QTest interface.  Each PictoDeviceSimulator runs in its
 *	own thread so as to truly simulate parallel activity of multiple devices.  The testbench
 *	adds a new action for a simulated device by calling its DoAction() function which blocks 
 *	until the previous action is finished, then starts the new action.  It can also wait for
 *	an action to end by using the WaitForActionComplete() call and can break out of the 
 *	the current action using the BreakCurrentAction() function.  Some convenience functions
 *	for common network activities are supplied for child classes, as well as the
 *	InterruptableSleep() function that child classes should use when sleeping to allow for
 *	the BreakCurrentAction() functionality to work correctly.
 */
class PictoDeviceSimulator : public QThread
{
	
public:
	PictoDeviceSimulator(PictoSystemState* systemState, QString deviceName);
	~PictoDeviceSimulator();
	
	QString GetDeviceName();
	virtual QString GetDeviceTypeName() = 0;
	void DoAction(QSharedPointer<SimActionDesc> actionDesc);
	void WaitForActionComplete();
	void BreakCurrentAction();
	void Close();
	bool isBeingTested;
protected:
	/*!	\brief This is called once when the PictoDeviceSimulator's thread is first started.
	 *	It can be overriden to initialize class members within the thread where they will
	 *	be active.
	 */
	virtual void Init() = 0;
	/*!	\brief This is called once when the PictoDeviceSimulator's thread is closed.
	 *	It can be overridden to destory any variables that need to be deleted within the thread
	 *	where they were started.
	 */
	virtual void Deinit() = 0;
	/*! \brief This function is where the PictoDeviceSimulator child object does all of its work.
	 *	This function should accept the SimActionDesc passed in and perform the action(s) defined 
	 *	in it.  Once the defined actions are complete, it should return.  It should also return
	 *	if an internal call to InterruptableSleep() returns true.  Error checking needs to occur
	 *	in this function as well, either automatically or as a result of some type of error checking 
	 *	simActionDesc input.
	 */
	virtual void Act(QSharedPointer<SimActionDesc> actionDesc) = 0;
	
	bool InterruptableSleep(int mSec);
	void ConnectToHost(QSharedPointer<QTcpSocket> tcpSocket, QHostAddress hostAddr,quint16 hostPort);
	void SendMessage(QString message, QSharedPointer<QTcpSocket> tcpSocket);
	void ReadIncomingMessage(QString expected, QString& message, QSharedPointer<QTcpSocket> tcpSocket, int timeout = 10000, bool requireMessage = true);
	/*! \brief A pointer to the PictoSystemState of the Picto System with which this device is associated.
	 *	This object can be used for error checking (ie. For checking the state of other objects to determine 
	 *	expected responses.
	 */
	PictoSystemState* systemState_;

private:
	virtual void run();
	QString deviceName_;
	QSharedPointer<SimActionDesc> actionDesc_;
	bool newActionReady_;
	bool keepRunning_;
	bool breakAction_;
	bool threadDone_;
};

}; // PictoSim namespace

#endif