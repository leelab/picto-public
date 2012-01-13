#ifndef ProxyStatusManager_H
#define ProxyStatusManager_H

#include "../../common/network/ComponentStatusManager.h"
#include "../../common/network/CommandChannel.h"
#include "NeuralDataAcqInterface.h"

class ProxyStatusManager : public ComponentStatusManager
{
public:
	ProxyStatusManager(QSharedPointer<Picto::CommandChannel> commandChannel);
	void setPlugin(QObject* acqPlugin){acqPlugin_ = qobject_cast<NeuralDataAcqInterface *>(acqPlugin);};
	NeuralDataAcqInterface* getPlugin(){return acqPlugin_;};
protected:
	virtual void newSession();
	virtual void doServerUpdate();
private:
	QSharedPointer<Picto::CommandChannel> commandChannel_;
	NeuralDataAcqInterface* acqPlugin_;
};
#endif