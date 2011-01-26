#include "PictoDeviceSimulator.h"
#include <QTime>
using namespace PictoSim;

PictoDeviceSimulator::PictoDeviceSimulator(PictoSystemState* systemState, QString deviceName)
{
	systemState_ = systemState;
	deviceName_ = deviceName;
	isBeingTested = true;
	newActionReady_ = false;
	keepRunning_ = true;
	breakAction_ = false;
	threadDone_ = false;
	start();
}

PictoDeviceSimulator::~PictoDeviceSimulator()
{
	Close();
}

QString PictoDeviceSimulator::GetDeviceName()
{
	return deviceName_;
}

/*!	\brief Waits for the previous action to complete, then starts this action.
 *	If the actionDesc includes a true breakPrevAction_ flag, the previous action
 *	is broken, after which this action is run.
 */
void PictoDeviceSimulator::DoAction(QSharedPointer<SimActionDesc> actionDesc)
{
	if(actionDesc->breakPrevAction_)
		BreakCurrentAction();
	WaitForActionComplete();
	actionDesc_ = actionDesc;
	newActionReady_ = true;
}

/*!	\brief Waits for the previous action to complete.
 */
void PictoDeviceSimulator::WaitForActionComplete()
{
	while(newActionReady_)yieldCurrentThread();
}

/*!	\brief Breaks the previous action processing and returns when the previous action is done.
 *	This assumes that the previous action processing sleeps using the InterruptableSleep() call.
 *	If it doesn't use this call the BreakCurrentAction() will function exactly like WaitForActionComplete()
 */
void PictoDeviceSimulator::BreakCurrentAction()
{
	while(newActionReady_)
	{
		breakAction_ = true;
		yieldCurrentThread();
	}
}

/*!	\brief Breaks the previous action processing, ends all activity for this PictoDeviceSimulator and returns.
 *	This call blocks until all activity has ended.
 */
void PictoDeviceSimulator::Close()
{
	keepRunning_ = false;
	while(!threadDone_) yieldCurrentThread();
}

/*!
 * \brief Puts the thread to sleep for mSec milliseconds or until the device has been asked to exit its Act() function.  
 *	If the device has been asked to exit its Act() funtion by the Close() or BreakCurrentAction() calls, 
 *	InterruptableSleep() returns true.  If it does return true, it is the child class's responsibility to break out
 *	of its Act() function.
 */
bool PictoDeviceSimulator::InterruptableSleep(int mSec)
{
	QTime timer;
	timer.start();
	while(!breakAction_ && keepRunning_ && (timer.elapsed() < mSec))
	{
		msleep(1);
	}
	return !keepRunning_ || breakAction_;
}

/*!	\brief This function loops in its own thread until Close() is called, calling Act() whenever a new action is available.
 *	When an Act() call is finished, the function waits for the next action to be ready.
 */
void PictoDeviceSimulator::run()
{
	Init();
	while(keepRunning_)
	{
		if(!newActionReady_)
		{
			breakAction_ = false;
			yieldCurrentThread();
			continue;
		}
		Act(actionDesc_);
		newActionReady_ = false;
	}
	Deinit();
	threadDone_ = true;
}

/*!	\brief A convenience function that a child class can use to connect a tcpSocket to a host with error checking.
 */
void PictoDeviceSimulator::ConnectToHost(QSharedPointer<QTcpSocket> tcpSocket, QHostAddress hostAddr,quint16 hostPort)
{
	tcpSocket->connectToHost(hostAddr,hostPort);
	if(!tcpSocket->waitForConnected(10000))
	{
		QFAIL((GetDeviceTypeName() + " failed to connect to port: " + hostPort + ", address: " + hostAddr.toString()).toAscii());
	}
}

/*!	\brief A convenience function that a child class can use to send a message over a tcpSocket to a host with error checking.
 */
void PictoDeviceSimulator::SendMessage(QString message, QSharedPointer<QTcpSocket> tcpSocket)
{
	if(!tcpSocket->waitForConnected(0))
	{
		QFAIL((GetDeviceTypeName() + " Can't send its message.  Tcp socket is not connected").toAscii() );
	}
	if(tcpSocket->write(message.toAscii()) == -1)
	{
		QFAIL((GetDeviceTypeName() + " Can't send its message.  Tcp socket failed to send message.").toAscii() );
	}
}

/*!	\brief A convenience function that a child class can use to read an incoming message over a tcpSocket with error checking.
 *	The expected parameter is the string that the message is expected to start with.  The message is returned in the "message"
 *	parameter.  if requireMessage is true, the function will report a failed test if timeout goes by without an incoming message.
 */
void PictoDeviceSimulator::ReadIncomingMessage(QString expected, QString& message, QSharedPointer<QTcpSocket> tcpSocket, int timeout, bool requireMessage)
{
	message = "";
	if(!tcpSocket->waitForConnected(0))
	{
		tcpSocket->close();
		QFAIL((GetDeviceTypeName() +" Can't read incoming messages.  Tcp socket is not connected").toAscii());
	}

	if(tcpSocket->waitForReadyRead(timeout))
	{
		message = tcpSocket->readAll();
	}
	else
	{
		if(requireMessage)
			QFAIL((GetDeviceTypeName() +" timed out while waiting for the following message: " + expected).toAscii());
	}

	if(requireMessage && !message.startsWith(expected))
	{
		QVERIFY2(message == expected, (GetDeviceTypeName() +" received an unexpected or badly formed message.\nActual: " 
			+ message + "\nExpected: " + expected).toAscii());
	}
}