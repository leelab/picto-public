#ifndef CONTROLPANELINTERFACE_H_
#define CONTROLPANELINTERFACE_H_

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>

#include "../../common/protocol/protocolcommand.h"
#include "../../common/protocol/protocolresponse.h"
#include "../../common/protocol/protocolcommandhandler.h"

/*! \brief The interface between the Picto Director and a Hardware Control panel.
 *	\details A hardware control panel controls the Director name, reward/flush durations, and offers the 
 *	capability to start a reward or start/stop a flush.  This class provides interfaces for all of 
 *	these capabilities and should be inherited to implement that interface for particular types of hardware.
 *	The Picto Engine uses this class to inform the front panel when these Director properties have changed,
 *	and to change these Director properties when asked by the control panel to do so.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API ControlPanelInterface : public QObject
#else
class ControlPanelInterface : public QObject
#endif
{
	Q_OBJECT

public:
	ControlPanelInterface();
	~ControlPanelInterface();

public slots:
	//These functions are called by the director whenever director state data
	//changes.  When they are called, data is updated in this object and then
	//child classes are informed of the change.

	/*! \brief Call this to tell the control panel that the Director's name changed.
	 *	\details Internally, this function calls nameWasChanged().
	 */
	void nameChanged(QString name){name_ = name;nameWasChanged();};
	/*! \brief Call this to tell the control panel that the Director's ip address changed.
	 *	\details Internally, this function calls ipWasChanged().
	 *	\note Currently, Picto doesn't have any way to actually physically change the Director's IP Address.  
	 *	This function implies that someone might be changing the Director IP Address, but it is actually
	 *	only used when a ControlPanelInterface is first created in order to tell it what the Director's
	 *	IP Address is.
	 */
	void ipAddressChanged(QString addr){ipAddress_ = addr;ipWasChanged();};
	void rewardDurationChanged(int controller, int duration);
	void flushDurationChanged(int controller, int duration);
	/*! \brief Call this to tell the control panel that the Director's status has changed.
	 *	\details Internally, this function calls statusWasChanged().
	 */
	void statusChanged(QString status){status_ = status;statusWasChanged(status);};
	/*! \brief Calling this function causes this control panel to process incoming commands from control panel hardware.
	 *	\details Processing the incoming commands results in calling the various "set" commands (ie. setName(), 
	 *	setRewardDuration(), setFlushDuration(), flush(), giveReward()) which in turn emit various signals 
	 *	(nameChangeRequest(), rewardDurationChangeRequest(), flushDurationChangeRequest(), flushRequest(), giveRewardRequest()) 
	 *	so that objects that connect to this one will be informed of new commands coming in from the ControlPanel.
	 */
	virtual void doIncomingCommands() = 0;

signals:
	//These signals are used to inform whoever's interested that the control panel
	//would like to change the director state data.
	
	/*! \brief Emitted when the ControlPanel hardware requests that the Director's name be changed.*/
	void nameChangeRequest(QString name);
	/*! \brief Emitted when the ControlPanel hardware requests that the Director's reward duration be changed to the duration number of milliseconds
	 *	for the input controller.
	 */
	void rewardDurationChangeRequest(int controller, int duration);
	/*! \brief Emitted when the ControlPanel hardware requests that the Director's flush duration be changed to the duration number of seconds
	 *	for the input controller.
	 */
	void flushDurationChangeRequest(int controller, int duration);
	/*! \brief Emitted when the ControlPanel hardware requests that the Director begin a flush on the input controller.*/
	void flushRequest(int controller);
	/*! \brief Emitted when the ControlPanel hardware requests that the Director begin a reward on the input controller.*/
	void giveRewardRequest(int controller);

protected:
	//When state data changes in the director, after that data is recorded by this
	//object, these functions are called on the child objects to inform them of the change.

	/*! \brief Called when the Director name was changed.  This function informs the control panel hardware of this change.
	 *	\sa nameChanged()
	 */
	virtual void nameWasChanged()=0;
	/*! \brief Called when the Director IP Address was changed.  This function informs the control panel hardware of this change.
	 *	\sa ipAddressChanged()
	 */
	virtual void ipWasChanged()=0;
	/*! \brief Called when the Director's reward duration was changed on the input controller.  This function informs the control panel hardware of this change.
	 *	\sa rewardDurationChanged()
	 */
	virtual void rewardDurationWasChanged(int controller)=0;
	/*! \brief Called when the Director's flush duration was changed on the input controller.  This function informs the control panel hardware of this change.
	 *	\sa flushDurationChanged()
	 */
	virtual void flushDurationWasChanged(int controller)=0;
	/*! \brief Called when the Director's status duration was changed .  This function informs the control panel hardware of this change.
	 *	\sa statusChanged()
	 */
	virtual void statusWasChanged(QString status)=0;

	//These functions are used by child objects to get current director state data

	/*! \brief Returns the current name of the Director.*/
	QString getName() { return name_; };
	/*! \brief Returns the current IP Address of the Director.*/
	QString getIP() { return ipAddress_; };
	int getRewardDuration(int controller);
	int getFlushDuration(int controller);
	/*! \brief Returns the current status of the Director.*/
	QString getStatus(){return status_;};

protected slots:
	//These functions are called by child classes to request changes in the director state data.

	/*! \brief Called during doIncomingCommands() to change the Director name if a request comes in from the Control Panel hardware to do so.*/
	void setName(QString name){emit nameChangeRequest(name);};
	/*! \brief Called during doIncomingCommands() to change the Director's Reward Duration on the input controller if a request comes in from 
	 *	the Control Panel hardware to do so.
	 *	duration is the duration in milliseconds of the reward.
	 */
	void setRewardDuration(int controller, int duration){emit rewardDurationChangeRequest(controller,duration);};
	/*! \brief Called during doIncomingCommands() to change the Directors Flush Duration on the input controller if a request comes in from 
	 *	the Control Panel hardware to do so.
	 *	duration is the duration in seconds of the flush.
	 */
	void setFlushDuration(int controller, int duration) {emit flushDurationChangeRequest(controller,duration);};
	/*! \brief Called during doIncomingCommands() to start or stop a flush on the Director's input controller if a request comes in from the 
	 *	Control Panel hardware to do so.
	 *	\details flush() starts a Flush if there was not yet one in progress and stops it if there was one in progress.
	 */
	void flush(int controller){emit flushRequest(controller);};
	/*! \brief Called during doIncomingCommands() to supply a reward on the Director's input controller.
	 *	\details Unlike flush(), rewards are queued if one is requested before the previous one finishes.  Reward duration is according to 
	 *	the value set in setRewardDuration() or previously saved on the director.
	 */
	void giveReward(int controller){emit giveRewardRequest(controller);};

private:
	QString name_;
	QString ipAddress_;
	QVector<int> rewardDurations_;
	QVector<int> flushDurations_;
	QString status_;
};

#endif