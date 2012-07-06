#ifndef _DOUBLEPARAMETER_H_
#define _DOUBLEPARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*!	\brief A parameter for containing numeric values.
 *
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API DoubleParameter : public Parameter
#else
class DoubleParameter : public Parameter
#endif
{
	Q_OBJECT
	Q_PROPERTY(double value READ getValue WRITE setValue)

public:
	DoubleParameter();
	virtual ~DoubleParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();

	double getValue();
	void setValue(double val);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool fixValues(QString& warning);

private:
	void checkForPropertyChanges();
};


}; //namespace Picto

#endif
