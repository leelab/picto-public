#ifndef _StringVariable_H_
#define _StringVariable_H_

#include "../common.h"

#include "Variable.h"

namespace Picto {

/*! \brief A Variant Variable.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API StringVariable : public Variable
#else
class StringVariable : public Variable
#endif
{
	Q_OBJECT
	Q_PROPERTY(QString value READ getValue WRITE setValue)

public:
	StringVariable();
	virtual ~StringVariable(){};

	static QSharedPointer<Asset> Create();

	QString getValue(){return propertyContainer_->getPropertyValue("Value").toString();};
	void setValue(QString val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString friendlyTypeName(){return "String Variable";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
