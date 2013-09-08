#ifndef _PSEUDOPseudorandomIntParameter_H_
#define _PSEUDOPseudorandomIntParameter_H_

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
	class PICTOLIB_API PseudorandomIntParameter : public Parameter
#else
class PseudorandomIntParameter : public Parameter
#endif
{
	Q_OBJECT
	Q_PROPERTY(int value READ getValue WRITE setValue)
	//Note: changing the values of min or max cause this object to reset its data.
	Q_PROPERTY(int min READ getMin WRITE setMin)
	Q_PROPERTY(int max READ getMax WRITE setMax)
public slots:
	void randomize();
	void reshuffleLastValue();
	void reset();

public:
	PseudorandomIntParameter();
	virtual ~PseudorandomIntParameter(){};

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create();
	virtual QString friendlyTypeName(){return "Pseudorandom Int";};
	virtual QString getUITemplate(){return "PseudorandomInt";};

	int getValue(){return propertyContainer_->getPropertyValue("Value").toInt();};
	void setValue(int val){propertyContainer_->setPropertyValue("Value",val);};

	virtual bool valuesAreValid(QString& warning = QString());
	virtual void fixValues();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void checkForPropertyChanges();
	int getMin();
	//Note: changing the values of min or max cause this object to reset its data.
	void setMin(int min);
	int getMax();
	//Note: changing the values of min or max cause this object to reset its data.
	void setMax(int max);
	MTRand mtRand_;
	QVector<int> randomArray_;
	bool useSeed_;
	int seed_;
	int value_;
	int min_;
	int max_;
	QString units_;
	int currIndex_;
};


}; //namespace Picto

#endif
