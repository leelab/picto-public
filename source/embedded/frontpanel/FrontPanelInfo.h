
#ifndef FRONTPANEL_H
#define FRONTPANEL_H

#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QHostAddress>
#include <QTcpSocket>

/**
 *	\brief A namespace containing enumerators used throughout the Front Panel control system
 */
namespace PanelInfo
{
	/**
	 * A complete listing of the types of DisplayMode object used in the Front Panel System
	 */
	enum DisplayModeType{MenuModeType, StatusModeType, RewardDurModeType, RewardControllerModeType, 
		FlushDurModeType, ChangeNameModeType};
	/**
	 * A complete listing of the types of user inputs that the DisplayMode objects can respond to.
	 */
	enum InputType{buttonPush, rotateLeft, rotateLeftFast, rotateRight, rotateRightFast,rewardButton,flushButton};
}

/*!	\brief Stores the command socket used by the Front Panel to communicate with the director as well as the current reward controller being used.
 *
 * This class used to be much more important.  It could probably be removed and have its parts incorporated
 * in other locations.  In the meantime, it's still here.  Note that the eventSocket is no longer used.  The
 * DirectorInterface is the chief user of the commandSocket and rewardController data stored here.
 * \sa DirectorInterface
 * \author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 * \date 2009-2015
 */
class FrontPanelInfo
{
public:
	FrontPanelInfo();
	virtual ~FrontPanelInfo() {};

	/*! \brief Returns the index of the currently active local reward controller
	 * \sa  DirectorInterface::DirectorInterface(), DirectorInterface::startReward()
	 */
	int getRewardController() { return rewardController; };
	/*! \brief Sets the index of the currently active local reward controller
	 * \sa  DirectorInterface::DirectorInterface(), DirectorInterface::startReward()
	 */
	void setRewardController(int controller) { rewardController = controller; };
	
	/*! \brief Returns the command socket that is used by the Front Panel Application to communicate with the Director.
	 * \sa DirectorInterface
	 */
	QTcpSocket* getCommandSocket() { return commandSocket; }
	/*! \brief Returns the command socket that is used by the Front Panel Application to communicate with the Director.
	 * \sa DirectorInterface
	 */
	void setCommandSocket(QTcpSocket* socket) { commandSocket = socket; };

	/*! \brief NO LONGER USED */
	QTcpSocket* getEventSocket() { return eventSocket; }
	/*! \brief NO LONGER USED */
	void setEventSocket(QTcpSocket* socket) { eventSocket = socket; };


private:
	int rewardController;

	QTcpSocket *commandSocket;
	QTcpSocket *eventSocket;
};
#endif