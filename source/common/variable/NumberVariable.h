#ifndef _NumberVariable_H_
#define _NumberVariable_H_

#include "../common.h"

#include "Variable.h"

namespace Picto {

/*! \brief A Variant Variable.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API NumberVariable : public Variable
#else
class NumberVariable : public Variable
#endif
{
	Q_OBJECT
	Q_PROPERTY(double value READ getValue WRITE setValue)

public:
	NumberVariable();
	virtual ~NumberVariable(){};

	static QSharedPointer<Asset> Create();

	double getValue(){return propertyContainer_->getPropertyValue("Value").toDouble();};
	void setValue(double val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString getUITemplate(){return "NumberVariable";};
	virtual QString friendlyTypeName(){return "Number Variable";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
