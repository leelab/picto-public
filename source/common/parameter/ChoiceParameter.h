#ifndef _CHOICEPARAMETER_H_
#define _CHOICEPARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

struct ChoiceParameterOption
{
	QString type;
	QVariant data;
};

#if defined WIN32 || defined WINCE
	class PICTOLIB_API ChoiceParameter : public Parameter
#else
class ChoiceParameter : public Parameter
#endif
{
public:
	ChoiceParameter();

	static Parameter* NewParameter();

	//DataStore functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	bool addChoice(QString label, QVariant data);
	bool removeChoice(QString label);
	void setDefaultOption(QString label);

	virtual void setValue(QVariant value);
	virtual QVariant getValue();

	virtual bool equalTo(Parameter& RHS);
	virtual bool equalTo(QVariant& RHS);

private:
	QMap<QString,ChoiceParameterOption> options_;
	QString currentOption_;
	QString defaultOption_;

};


}; //namespace Picto

#endif
