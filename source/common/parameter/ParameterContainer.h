#ifndef _PARAMETERCONTAINER_H_
#define _PARAMETERCONTAINER_H_

#include "../common.h"
#include "../statemachine/ResultContainer.h"

#include "Parameter.h"

#include <QSharedPointer>
#include <QList>

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
class PICTOLIB_API ParameterContainer : public ResultContainer
#else
class ParameterContainer : public ResultContainer
#endif
{
	Q_OBJECT
public:
	ParameterContainer();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new ParameterContainer());};

	//QSharedPointer<Parameter> getParameter(QString name);
	virtual QString assetType(){return "ParameterContainer";};

protected:
	virtual QString defaultTagName(){return "Parameters";};
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
