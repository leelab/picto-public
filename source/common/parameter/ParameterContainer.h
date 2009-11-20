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
	void setParameterValue(QString _parameterName, QVariant _value);
	void setContainerName(QString _containerName);

signals:
	void signalParameterValueChanged(QString parameterName, QVariant parameterValue);

private:
	QtVariantPropertyManager variantManager_;
	QtVariantProperty * containerGroupItem_;
	QMap<QString, QtVariantProperty *> parameters_;

private slots:
	void slotPropertyManagerValueChanged(QtVariantProperty * property, const QVariant & value);
};


}; //namespace Picto

#endif
