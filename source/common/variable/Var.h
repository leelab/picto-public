#ifndef _Var_H_
#define _Var_H_

#include "../common.h"

#include "Variable.h"

namespace Picto {

/*! \brief A Variant Variable.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API Var : public Variable
#else
class Var : public Variable
#endif
{
	Q_OBJECT
	Q_PROPERTY(QVariant value READ getValue WRITE setValue)

public:
	Var();
	virtual ~Var(){};

	static QSharedPointer<Asset> Create();

	QVariant getValue(){return propertyContainer_->getPropertyValue("Value");};
	void setValue(QVariant val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString friendlyTypeName(){return "Var";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
