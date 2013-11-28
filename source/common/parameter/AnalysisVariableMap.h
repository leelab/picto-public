#ifndef _AnalysisVariableMap_H_
#define _AnalysisVariableMap_H_

#include "../common.h"

#include "AnalysisVariable.h"

namespace Picto {

/*! \brief A VariableMap for Analyses.
 *	\details Works exactly like a VariableMap with exception that it is part of an Analysis.
 *	\sa VariableMap
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisVariableMap : public AnalysisVariable
#else
class AnalysisVariableMap : public AnalysisVariable
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
		
	/*! \brief Returns the value under the input key as a double.*/
	double getValueAsNum(QVariant key){return getValue(key.toString()).toDouble();};
	/*! \brief Returns the value under the input key as a string.*/
	QString getValueAsString(QVariant key){return getValue(key.toString()).toString();};
	QVariantList getKeys();
	
	/*! \brief Returns the value under the input key as a double and removes it.*/
	double takeAtAsNum(QVariant key){return takeAt(key.toString()).toDouble();};
	/*! \brief Returns the value under the input key as a string and removes it.*/
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
