#ifndef ProxyStatusManager_H
#define ProxyStatusManager_H

#include "../../common/network/ComponentStatusManager.h"
#include "../../common/network/CommandChannel.h"
#include "NeuralDataAcqInterface.h"

/*! \brief Handles overall status operations for a ProxyMainWindow object.
 *	\details This class adds to the default ComponentStatusManager functionality by 
 *	implimenting doServerUpdate() to send COMPONENTUPDATE commands to the Picto Server.  It also
 *	provides functions for setting/getting the neural data acquisition plugin that will be used to gather
 *	data from the Neural DAQ software for sending to the Server. It also provides a getSessionDBName()
 *  member used to display the name of a session database by a proxy server.
 *	\sa ComponentStatusManager
 *	\author Mike Scudder, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2019
 */
class ProxyStatusManager : public ComponentStatusManager
{
	Q_OBJECT
public:
	ProxyStatusManager(QSharedPointer<Picto::CommandChannel> commandChannel);
	/*! \brief Sets the plugin that will be used to gather data from the Neural Data Acquisition system for
	 *	sending to the PictoServer.
	 */
	void setPlugin(QObject* acqPlugin){acqPlugin_ = qobject_cast<NeuralDataAcqInterface *>(acqPlugin);};
	/*! \brief Gets the plugin that is being used to gather data from the Neural Data Acquisition system for
	 *	sending to the PictoServer.
	 */
	NeuralDataAcqInterface* getPlugin(){return acqPlugin_;};
	/*! \brief Sets the name of the object.*/
	void setName(QString name){name_ = name;};
	virtual void setUserInfo(QString info);
	virtual QString getName(){return name_;};
	virtual QString getSessionDBName(){ return sessionDBName_;};
protected:
	virtual void newSession();
	virtual void doServerUpdate();
private:
	QString sessionDBPath_;
	QString sessionDBName_;
	QString name_;
	QSharedPointer<Picto::CommandChannel> commandChannel_;
	NeuralDataAcqInterface* acqPlugin_;
};
#endif