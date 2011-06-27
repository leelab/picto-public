#ifndef _BOOLEANPARAMETER_H_
#define _BOOLEANPARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*! \brief A boolean parameter.
 *
 *	This parameter can only hold the values true or false, although we can relable those
 *	values to anything we want (e.g. "On" and "Off").  The equality operators work as 
 *	expected for these parameters, but the greater than and less than operators will
 *	always return false, since theydoesn't make sense with boolean values.
 */
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
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new BooleanParameter());};

	//DataStore functions
	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void setDefaultValue(bool defaultValue) { defaultValue_ = defaultValue; };
	void setTrueLabel(QString label) { trueLabel_ = label; };
	void setFalseLabel(QString label) { falseLabel_ = label; };

	//note that the lessThan & greaterThan functions aren't redefined, 
	//so they will always return false
	bool equalTo(Parameter& RHS) { return RHS.getValue().toBool() == value_ && RHS.type() == "Boolean"; };
	bool equalTo(QVariant& RHS) { return (RHS.type() == QVariant::Bool) && (RHS.toBool() == value_); };

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	bool value_;
	bool defaultValue_;
	QString trueLabel_;
	QString falseLabel_;

};


}; //namespace Picto

#endif
