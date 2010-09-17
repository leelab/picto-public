#ifndef _PARAMETERCONTAINER_H_
#define _PARAMETERCONTAINER_H_

#include "../common.h"
#include "../storage/DataStore.h"

#include "Parameter.h"

#include <QSharedPointer>
#include <QList>
#include <QScriptEngine>

namespace Picto {

/*! \brief A container for storing parameters
 *
 *	The parameter container is basically a list of parameters.  This makes it easy to 
 *	give parameters scope (if the parameter is in the container, it is in scope).  Note
 *	that the container actually stores QSharedPointers to the parameters which means 
 *	that we don't need to worry about deleting them, and can have multiple copies of a 
 *	parameter pointer.
 */
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

	void bindToScriptEngine(QScriptEngine &engine);


private:
	QMap<QString, QSharedPointer<Parameter> > parameters_;
};


}; //namespace Picto

#endif
