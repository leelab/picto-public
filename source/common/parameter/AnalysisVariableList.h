#ifndef _AnalysisVariableList_H_
#define _AnalysisVariableList_H_

#include "../common.h"

#include "AnalysisVariable.h"

namespace Picto {

/*! \brief A Variant Variable.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisVariableList : public AnalysisVariable
#else
class AnalysisVariableList : public Variable
#endif
{
	Q_OBJECT

public:
	AnalysisVariableList();
	virtual ~AnalysisVariableList(){};

	static QSharedPointer<Asset> Create();

	virtual void enteredScope();

	//Resets the AnalysisVariableList Object to its initial state.
	virtual QString getUITemplate(){return "ListVariable";};
	virtual QString friendlyTypeName(){return "Variable List";};
	
public slots:
	int length();
	void append(QVariant value);
	void prepend(QVariant value);
	void setValue(int index,QVariant value);
	void fromArray(QVariantList array);

	double getValueAsNum(int index){return getValue(index).toDouble();};
	double firstAsNum(){return first().toDouble();};
	double lastAsNum(){return last().toDouble();};

	double takeFirstAsNum(){return takeFirst().toDouble();};
	double takeLastAsNum(){return takeLast().toDouble();};
	double takeAtAsNum(int index){return takeAt(index).toDouble();};

	QString getValueAsString(int index){return getValue(index).toString();};
	QString firstAsString(){return first().toString();};
	QString lastAsString(){return last().toString();};

	QString takeFirstAsString(){return takeFirst().toString();};
	QString takeLastAsString(){return takeLast().toString();};
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
