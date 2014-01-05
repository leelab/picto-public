#ifndef _AnalysisVariableList_H_
#define _AnalysisVariableList_H_

#include "../common.h"

#include "AnalysisVariable.h"

namespace Picto {

/*! \brief A VariableList for Analyses.
 *	\details Works exactly like the VariableList with exception that it is part of an Analysis.
 *	\sa VariableList
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisVariableList : public AnalysisVariable
#else
class AnalysisVariableList : public AnalysisVariable
#endif
{
	Q_OBJECT

public:
	AnalysisVariableList();
	virtual ~AnalysisVariableList(){};

	static QSharedPointer<Asset> Create();

	virtual void enteredScope();

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

	QVariantList list_;
};


}; //namespace Picto

#endif
