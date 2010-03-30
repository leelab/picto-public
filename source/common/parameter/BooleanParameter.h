#ifndef _BOOLEANPARAMETER_H_
#define _BOOLEANPARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

#if defined WIN32 || defined WINCE
	class PICTOLIB_API BooleanParameter : public Parameter
#else
class BooleanParameter : public Parameter
#endif
{
	Q_OBJECT
public slots:
	void setValue(QVariant value) { value_ = value.toBool(); };
	QVariant getValue() { return QVariant(value_); };

public:
	BooleanParameter();

	static Parameter* NewParameter();

	//DataStore functions
	bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void setDefaultValue(bool defaultValue) { defaultValue_ = defaultValue; };
	void setTrueLabel(QString label) { trueLabel_ = label; };
	void setFalseLabel(QString label) { falseLabel_ = label; };

	//note that the lessThan & greaterThan functions aren't redefined, 
	//so they will always return false
	bool equalTo(Parameter& RHS) { return RHS.getValue().toBool() == value_ && RHS.type() == "Boolean"; };
	bool equalTo(QVariant& RHS) { return (RHS.type() == QVariant::Bool) && (RHS.toBool() == value_); };

private:
	bool value_;
	bool defaultValue_;
	QString trueLabel_;
	QString falseLabel_;

};


}; //namespace Picto

#endif