#include <QVector>
#include <QHostAddress>
#include <QSharedPointer>
#include "PictoDeviceSimulator.h"
namespace PictoSim{

/*! \brief 
 */
class PictoServerSimulator : public PictoDeviceSimulator
{
public:
	PictoServerSimulator(PictoSystemState* systemState, QString deviceName);
	virtual QString GetDeviceTypeName();
	struct SessionSimulator
	{
		unsigned int sessionID_;
		QSharedPointer<unsigned int> authorizedWorkstation_;
		QVector<QSharedPointer<unsigned int>> unAuthorizedWorkstations_;
		QSharedPointer<unsigned int> director_;
		QSharedPointer<unsigned int> proxyServer_;
	};
	QVector<SessionSimulator> sessions_;
	QMap<QString,QSharedPointer<PictoDeviceSimulator>> knownDirectors_;
protected:
	virtual void Init();
	virtual void Deinit();
	virtual void Act(QSharedPointer<SimActionDesc> actionDesc);
};

namespace Server{
	enum DescTypes{
	};

};// Server namespace

}; // PictoSim namespace