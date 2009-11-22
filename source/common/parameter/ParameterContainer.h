#ifndef _PARAMETERCONTAINER_H_
#define _PARAMETERCONTAINER_H_

#include "../common.h"

#include "Parameter.h"

#include <QObject>

namespace Picto {

class ParameterContainer : public QObject
{
	Q_OBJECT

public:
	ParameterContainer();

//signals:
//	void signalParameterValueChanged(QString parameterName, QVariant parameterValue);

//private slots:
//	void slotPropertyManagerValueChanged(QtProperty * property, const QVariant & value);
};


}; //namespace Picto

#endif
