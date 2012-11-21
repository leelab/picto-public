#ifndef _STATEUPDATER_H_
#define _STATEUPDATER_H_
#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>

#include "../common.h"

namespace Picto {

/*! StateUpdater
 *
 *	Parent class for objects that update the Picto State
 *	from some external source
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API StateUpdater : public QObject
#else
class StateUpdater : public QObject
#endif
{
	Q_OBJECT
public:
	StateUpdater(){};
	virtual ~StateUpdater(){};
	//Updates the picto state to the latest values
	virtual bool updateState() = 0;
signals:
	//triggered when values change
	void propertyChanged(int propId, QString value);
	void transitionActivated(int transId);
	void framePresented(double time);
	void rewardSupplied(double time,int duration,int channel);
	void signalChanged(QString name,QStringList subChanNames,QVector<float> vals);
};


}; //namespace Picto

#endif
