#ifndef _RANDOMINTPARAMETER_H_
#define _RANDOMINTPARAMETER_H_

#include "../common.h"

#include "parameter.h"
#include "../random/MersenneTwister.h"

namespace Picto {

/*!	\brief A Parameter for selecting Random integer values
 *	\details Random values are generated when randomize() is called and available in the "value" script
 *	property.  The "value" script property can also be set to another value that will stay there until
 *	randomize() is called again, a new value is set to it or the RandomIntParameter re-enters scope.
 *	Random values generated by this function are between its "Min" (>=) and "Max" (<=) Property Values.
 *	Seed functionality is also provided in case a reproducable series of random numbers is desired. 
 *	\note In many cases, it is possible simply to use javascript like \code Math.floor(Math.random()) \endcode 
 *	to produce a simple random integer.  The produced number in that case will not be recorded though unless
 *	it is manually set to some Picto element value.
 *
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API RandomIntParameter : public Parameter
#else
class RandomIntParameter : public Parameter
#endif
{
	Q_OBJECT
	/*! \brief Returns the latest random value that was selected when randomize() was called.
	 *	\details Setting this value is also possible and will cause the set value to be returned from the value
	 *	script property until it is manually changed, or randomize() is next called.
	*/
	Q_PROPERTY(int value READ getValue WRITE setValue)
public slots:
	void randomize();

public:
	RandomIntParameter();
	virtual ~RandomIntParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();
	/*! \brief Returns the latest random value that was selected when randomize() was called
	 *	or that was set using setValue()
	*/
	int getValue(){return propertyContainer_->getPropertyValue("Value").toInt();};
	/*! \brief Sets the value that will be returned from getValue() until this function or randomize() is 
	 *	next called.
	*/
	void setValue(int val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString friendlyTypeName(){return "Random Integer";};
	virtual QString getUITemplate(){return "RandomInt";};
	
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
	int value_;
	QString units_;
};


}; //namespace Picto

#endif
