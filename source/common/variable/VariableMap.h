#ifndef _VariableMap_H_
#define _VariableMap_H_

#include "../common.h"

#include "Variable.h"

namespace Picto {

/*! \brief A variable map that is used for storing values in a lookup table.
 *	\details Objects of this class offer functions for adding and retrieving values as well as 
 *	initialization of the map from a javascript associative array (fromAssocArray()).  Get functions 
 *	can be used to retrieve items cast either as numbers or strings.  
 *
 *	Internally, for transferring the map data over the network from the master Experiment
 *	to the PictoServer, data is stored in a QVariantMap Property that converts the data into a "::" 
 *	separated list of pairs of stringified values.  A regular C++ QVariantMap is maintained as well
 *	to keep "get" operations efficient.  Internal code synchronizes the Property and C++ map whenever 
 *	a "set" operation occurs both within a single object and over the network (ie. Slave object updated 
 *	from changes in an object from the Master Expeirment).
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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

	void copyMapToValueProp();
	void copyValuePropToMap();

	QVariantMap map_;
	bool settingValueProp_;

private slots:
	void propValueChanged(Property*,QVariant);
};


}; //namespace Picto

#endif
