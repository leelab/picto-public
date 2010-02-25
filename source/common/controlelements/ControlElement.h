#ifndef _CONTROLELEMENT_H_
#define _CONTROLELEMENT_H_

#include "../common.h"

#include "../statemachine/StateMachineElement.h"

#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include <QVariant>

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API ControlElement : public QObject
#else
class ControlElement : public QObject
#endif
{
	Q_OBJECT

public:
	ControlElement();

signals:
	void controlEvent(bool succeeded, QMap<QString, QVariant> controlParameters);

private:
	QList<QString> conditions_;
	QMap<QString, QSharedPointer<StateMachineElement> > connections_;
};


}; //namespace Picto

#endif
