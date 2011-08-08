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
public slots:
	QVariant getLastValue();
	void setValue(QVariant value){};
	QVariant getValue();
	void redoValue(QVariant value);

public:
	PseudorandomIntParameter();

	static Parameter* NewParameter();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new PseudorandomIntParameter());};
	virtual QString assetType(){return "PseudorandomInt";};

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void checkForPropertyChanges();
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
