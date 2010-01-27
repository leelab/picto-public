#ifndef _PARAMETERCONTAINER_H_
#define _PARAMETERCONTAINER_H_

#include "../common.h"
#include "../storage/DataStore.h"

#include "Parameter.h"

#include <QSharedPointer>
#include <QList>

namespace Picto {

class ParameterContainer : public DataStore
{
public:
	ParameterContainer();

	void addParameter(QSharedPointer<Parameter> parameter);
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QList<QSharedPointer<Parameter> > parameters_;
};


}; //namespace Picto

#endif
