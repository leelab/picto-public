#include "Parameter.h"

namespace Picto {

Parameter::Parameter() :
	bOperatorUI_(false),
	name_("unnamed parameter"),
	type_("null"),
	order_(0)
{
}

QString Parameter::name()
{
	return name_;
}

void Parameter::setName(QString parameterName)
{
	name_ = parameterName;
}

QString Parameter::type()
{
	return type_;
}

void Parameter::bindToScriptEngine(QScriptEngine &engine)
{
	QScriptValue qsValue = engine.newQObject(this);
	engine.globalObject().setProperty(name(),qsValue);

}


}; //namespace Picto
