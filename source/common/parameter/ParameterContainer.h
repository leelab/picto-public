#ifndef _PARAMETERCONTAINER_H_
#define _PARAMETERCONTAINER_H_

#include "../common.h"

#include "Parameter.h"

#include <QtVariantPropertyManager>
#include <QString>

namespace Picto {

class ParameterContainer : public QObject
{
	Q_OBJECT

public:
	ParameterContainer(QString _containerName);

	void addParameter(Parameter _parameter);
	QVariant getParameterValue(QString _parameterName);

signals:
	void signalParameterValueChanged(QString parameterName, QVariant parameterValue);

private:
	QtVariantPropertyManager variantManager_;
	QtProperty * containerGroupItem_;
	QMap<QString, QtProperty *> parameters_;

private slots:
	void slotPropertyManagerValueChanged(QtProperty * property, const QVariant & value);
};


}; //namespace Picto

#endif
