#ifndef _StringVariable_H_
#define _StringVariable_H_

#include "../common.h"

#include "Variable.h"

namespace Picto {

/*! \brief A simple variable that is used for storing strings.
 *	\details The name pretty much says it all.  The object holds a string while it is in scope.
 *	The string can be read/set using the "value" script property.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API StringVariable : public Variable
#else
class StringVariable : public Variable
#endif
{
	Q_OBJECT
	/*! \brief A script property used to read and write this StringVariable's value.*/
	Q_PROPERTY(QString value READ getValue WRITE setValue)

public:
	StringVariable();
	virtual ~StringVariable(){};

	static QSharedPointer<Asset> Create();
	/*! \brief Returns this StringVariable's value.*/
	QString getValue(){return propertyContainer_->getPropertyValue("Value").toString();};
	/*! \brief Sets this StringVariable's value.*/	
	void setValue(QString val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString getUITemplate(){return "StringVariable";};
	virtual QString friendlyTypeName(){return "String Variable";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
