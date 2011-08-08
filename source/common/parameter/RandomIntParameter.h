#ifndef _RANDOMINTPARAMETER_H_
#define _RANDOMINTPARAMETER_H_

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
	class PICTOLIB_API RandomIntParameter : public Parameter
#else
class RandomIntParameter : public Parameter
#endif
{
	Q_OBJECT
public slots:
	QVariant getLastValue();
	void setValue(QVariant value){};
	QVariant getValue();

public:
	RandomIntParameter();

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new RandomIntParameter());};
	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

protected:
	virtual void postSerialize();
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
