#ifndef _VarMap_H_
#define _VarMap_H_

#include "../common.h"

#include "Variable.h"

namespace Picto {

/*! \brief A Variant Variable.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API VarMap : public Variable
#else
class VarMap : public Variable
#endif
{
	Q_OBJECT

public:
	VarMap();
	virtual ~VarMap(){};

	static QSharedPointer<Asset> Create();

	//Resets the VarList Object to its initial state.
	virtual void reset();

	virtual QString friendlyTypeName(){return "VarMap";};
	
public slots:
	int length();
	void setValue(QVariant key, QVariant value);
	void fromAssocArray(QVariantMap array);

	QVariant getValue(QVariant key);
	QVariantList getKeys();
	QVariantList getValues();
	QVariantMap toAssocArray();

	QVariant takeAt(QVariant key);
	
	void removeAt(QVariant key);


protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QVariantMap map_;
};


}; //namespace Picto

#endif
