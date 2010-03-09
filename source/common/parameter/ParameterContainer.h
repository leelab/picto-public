#ifndef _PARAMETERCONTAINER_H_
#define _PARAMETERCONTAINER_H_

#include "../common.h"
#include "../storage/DataStore.h"

#include "Parameter.h"

#include <QSharedPointer>
#include <QList>
#include <QScriptEngine>

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API ParameterContainer : public DataStore
#else
class ParameterContainer : public DataStore
#endif
{
public:
	ParameterContainer();

	void addParameter(QSharedPointer<Parameter> parameter);
	QSharedPointer<Parameter> getParameter(QString name);
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QStringList getParameterList();

	void addAsScriptProperties(QScriptEngine &engine);


private:
	QMap<QString, QSharedPointer<Parameter> > parameters_;
};


}; //namespace Picto

#endif
