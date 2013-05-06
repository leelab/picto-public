#ifndef _VarList_H_
#define _VarList_H_

#include "../common.h"

#include "Variable.h"

namespace Picto {

/*! \brief A Variant Variable.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API VarList : public Variable
#else
class VarList : public Variable
#endif
{
	Q_OBJECT

public:
	VarList();
	virtual ~VarList(){};

	static QSharedPointer<Asset> Create();

	//Resets the VarList Object to its initial state.
	virtual void reset();
	virtual QString friendlyTypeName(){return "VarList";};
	
public slots:
	int length();
	void append(QVariant value);
	void prepend(QVariant value);
	void setValue(int index,QVariant value);
	void fromArray(QVariantList array);

	QVariant getValue(int index);
	QVariant first();
	QVariant last();
	QVariantList toArray();

	QVariant takeFirst();
	QVariant takeLast();
	QVariant takeAt(int index);

	void removeFirst();
	void removeLast();
	void removeAt(int index);






protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QVariantList list_;
};


}; //namespace Picto

#endif
