#ifndef _VariableList_H_
#define _VariableList_H_

#include "../common.h"

#include "Variable.h"

namespace Picto {

/*! \brief A variable list that is used for storing lists of values.
 *	\details This is a useful object for storing any kind of list of values.  It offers functions
 *	for appending, prepending, inserting, etc, and can even be initialized directory from a javascript
 *	array using fromArray().  Get functions can be used to retrieve list items cast either as numbers
 *	or strings.  
 *
 *	Internally, for transferring the list data over the network from the master Experiment
 *	to the PictoServer, data is stored in a QVariantList Property that converts the data into a "::" 
 *	separated list of stringified values.  A regular C++ QVariantList is maintained as well
 *	to keep "get" operations efficient.  Internal code synchronizes the Property and C++ list whenever 
 *	a "set" operation occurs both within a single object and over the network (ie. Slave object updated 
 *	from changes in an object from the Master Expeirment).
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API VariableList : public Variable
#else
class VariableList : public Variable
#endif
{
	Q_OBJECT

public:
	VariableList();
	virtual ~VariableList(){};

	static QSharedPointer<Asset> Create();

	virtual QString getUITemplate(){return "ListVariable";};
	virtual QString friendlyTypeName(){return "Variable List";};
	
public slots:
	int length();
	void append(QVariant value);
	void prepend(QVariant value);
	void setValue(int index,QVariant value);
	void fromArray(QVariantList array);

	/*! \brief Returns the value at the input index as a double.*/
	double getValueAsNum(int index){return getValue(index).toDouble();};
	/*! \brief Returns the first value in the list as a double.*/
	double firstAsNum(){return first().toDouble();};
	/*! \brief Returns the last value in the list as a double.*/
	double lastAsNum(){return last().toDouble();};

	/*! \brief Returns the first value in the list as a double and removes it.*/
	double takeFirstAsNum(){return takeFirst().toDouble();};
	/*! \brief Returns the last value in the list as a double and removes it.*/
	double takeLastAsNum(){return takeLast().toDouble();};
	/*! \brief Returns the value at the input index as a double and removes it.*/
	double takeAtAsNum(int index){return takeAt(index).toDouble();};

	/*! \brief Returns the value at the input index as a string.*/
	QString getValueAsString(int index){return getValue(index).toString();};
	/*! \brief Returns the first value in the list as a string.*/
	QString firstAsString(){return first().toString();};
	/*! \brief Returns the last value in the list as a string.*/
	QString lastAsString(){return last().toString();};

	/*! \brief Returns the first value in the list as a string and removes it.*/
	QString takeFirstAsString(){return takeFirst().toString();};
	/*! \brief Returns the last value in the list as a string and removes it.*/
	QString takeLastAsString(){return takeLast().toString();};
	/*! \brief Returns the value at the input index as a string and removes it.*/
	QString takeAtAsString(int index){return takeAt(index).toString();};

	void removeFirst();
	void removeLast();
	void removeAt(int index);
	void clear();

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QVariant getValue(int index);
	QVariant first();
	QVariant last();
	QVariantList toArray();

	QVariant takeFirst();
	QVariant takeLast();
	QVariant takeAt(int index);

	void copyListToValueProp();
	void copyValuePropToList();

	QVariantList list_;
	bool settingValueProp_;

private slots:
	void propValueChanged(Property*,QVariant);
};


}; //namespace Picto

#endif
