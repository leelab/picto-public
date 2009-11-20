#ifndef _STATEMACHINEELEMENT_H_
#define _STATEMACHINEELEMENT_H_

#include "../common.h"

#include <QObject>
#include <QPoint>
#include <QRect>
#include <QSharedPointer>

namespace Picto {

class StateMachineElement : public QObject
{
	Q_OBJECT

public:
	StateMachineElement();

	virtual QPoint getDisplayLayoutPosition();

	virtual QRect getDisplayBoundingRect() = 0;
	virtual void draw() = 0;

protected:
	QPoint layoutPosition_;
};


}; //namespace Picto

#endif
