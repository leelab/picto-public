#ifndef ProxyStatusManager_H
#define ProxyStatusManager_H

#include "../../common/network/ComponentStatusManager.h"
#include "../../common/network/CommandChannel.h"

class ProxyStatusManager : public ComponentStatusManager
{
public:
	ProxyStatusManager(QSharedPointer<Picto::CommandChannel> commandChannel);
protected:
	virtual void newSession();
	virtual void doServerUpdate();
private:
	QSharedPointer<Picto::CommandChannel> commandChannel_;
};
#endif