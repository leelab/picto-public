#ifndef _RANDOMDOUBLEPARAMETER_H_
#define _RANDOMDOUBLEPARAMETER_H_

#include "../common.h"

#include "parameter.h"
#include "../random/MersenneTwister.h"

namespace Picto {

/*!	\brief A parameter for containing numeric values.
 *
 *	Given our experience with Picto, it seems reasonable to limit these
 *	parameters to integer values.  If a user requires a non-integer
 *	value (e.g. 0.135 seconds), they can usually get away with changing
 *	the units (e.g. 135 milliseconds).
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API RandomDoubleParameter : public Parameter
#else
class RandomDoubleParameter : public Parameter
#endif
{
	Q_OBJECT
	Q_PROPERTY(double value READ getValue WRITE setValue)
public slots:
	void randomize();

public:
	RandomDoubleParameter();
	virtual ~RandomDoubleParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();

	double getValue(){return propertyContainer_->getPropertyValue("Value").toDouble();};
	void setValue(double val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString friendlyTypeName(){return "Random Float";};
	virtual QString getUITemplate(){return "PseudorandomFloat";};
	
	virtual bool valuesAreValid(QString& warning = QString());
	virtual void fixValues();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void checkForPropertyChanges();
	MTRand mtRand_;
	bool useSeed_;
	int seed_;
	double value_;
	QString units_;
};


}; //namespace Picto

#endif
