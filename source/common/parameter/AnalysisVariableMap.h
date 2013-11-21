#ifndef _AnalysisVariableMap_H_
#define _AnalysisVariableMap_H_

#include "../common.h"

#include "AnalysisVariable.h"

namespace Picto {

/*! \brief An Analysis Variable Map.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisVariableMap : public AnalysisVariable
#else
class AnalysisVariableMap : public Variable
#endif
{
	Q_OBJECT

public:
	AnalysisVariableMap();
	virtual ~AnalysisVariableMap(){};

	static QSharedPointer<Asset> Create();

	virtual void enteredScope();

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
	void clear();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QVariant getValue(QVariant key);
	QVariant takeAt(QVariant key);

	QVariantMap map_;
};


}; //namespace Picto

#endif
