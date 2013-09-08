#ifndef _VariableMap_H_
#define _VariableMap_H_

#include "../common.h"

#include "Variable.h"

namespace Picto {

/*! \brief A Variant Variable.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API VariableMap : public Variable
#else
class VariableMap : public Variable
#endif
{
	Q_OBJECT

public:
	VariableMap();
	virtual ~VariableMap(){};

	static QSharedPointer<Asset> Create();

	virtual QString getUITemplate(){return "MapVariable";};
	virtual QString friendlyTypeName(){return "Variable Map";};
	
public slots:
	int length();
	void setValue(QVariant key, QVariant value);
	void fromAssocArray(QVariantMap assocArray);

	double getValueAsNum(QVariant key){return getValue(key.toString()).toDouble();};
	QString getValueAsString(QVariant key){return getValue(key.toString()).toString();};
	QVariantList getKeys();
	
	double takeAtAsNum(QVariant key){return takeAt(key.toString()).toDouble();};
	QString takeAtAsString(QVariant key){return takeAt(key.toString()).toString();};
	void removeAt(QVariant key);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QVariant getValue(QVariant key);
	QVariant takeAt(QVariant key);

	void copyMapToValueProp();
	void copyValuePropToMap();

	QVariantMap map_;
	bool settingValueProp_;

private slots:
	void propValueChanged(Property*,QVariant);
};


}; //namespace Picto

#endif
