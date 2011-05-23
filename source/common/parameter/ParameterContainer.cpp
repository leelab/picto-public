#include "ParameterContainer.h"
#include "ParameterFactory.h"
#include "BooleanParameter.h"
#include "ChoiceParameter.h"
#include "NumericParameter.h"
#include "RangeParameter.h"

namespace Picto {

ParameterContainer::ParameterContainer()
{
	QSharedPointer<SerializableFactory> paramFactory(new SerializableFactory());
	paramFactory->addSerializableType("Boolean",
		QSharedPointer<SerializableFactory>(new SerializableFactory(0,-1,SerializableFactory::NewSerializableFnPtr(BooleanParameter::Create))));
	paramFactory->addSerializableType("Choice",
		QSharedPointer<SerializableFactory>(new SerializableFactory(0,-1,SerializableFactory::NewSerializableFnPtr(ChoiceParameter::Create))));
	paramFactory->addSerializableType("Numeric",
		QSharedPointer<SerializableFactory>(new SerializableFactory(0,-1,SerializableFactory::NewSerializableFnPtr(NumericParameter::Create))));
	paramFactory->addSerializableType("Range",
		QSharedPointer<SerializableFactory>(new SerializableFactory(0,-1,SerializableFactory::NewSerializableFnPtr(RangeParameter::Create))));
	AddDefinableObjectFactory("Parameter",paramFactory);

}

void ParameterContainer::addParameter(QSharedPointer<Parameter> parameter)
{
	Q_ASSERT(!parameters_.contains(parameter->name()));
	parameters_.insert(parameter->name(),parameter);
}

QSharedPointer<Parameter> ParameterContainer::getParameter(QString name)
{
	return parameters_.value(name,QSharedPointer<Parameter>());
}

//! \brief Returns a list of all contained parameters
QStringList ParameterContainer::getParameterList()
{
	QStringList parameterList;

	QMap<QString, QSharedPointer<Parameter>>::const_iterator param = parameters_.begin();
	while (param != parameters_.end()) 
	{
		parameterList.append(param.key());
		++param;
	}
	return parameterList;
}

void ParameterContainer::bindToScriptEngine(QScriptEngine &engine)
{
	foreach(QSharedPointer<Parameter> param, parameters_)
	{
		param->bindToScriptEngine(engine);
	}
}

//bool ParameterContainer::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	xmlStreamWriter->writeStartElement("Parameters");
//
//	//! \TODO: Add code for outputting SessionData id values
//
//	//serialize each parameter in the container
//	foreach(QSharedPointer<Parameter> p, parameters_)
//	{
//		p->toXml(xmlStreamWriter);
//	}
//
//	xmlStreamWriter->writeEndElement(); //Parameters
//	return true;
//}
//
//bool ParameterContainer::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	//Do some basic error checking
//	if(!xmlStreamReader->isStartElement() || xmlStreamReader->name() != "Parameters")
//	{
//		addError("ParameterContainer","Incorrect tag, expected <Parameters>",xmlStreamReader);
//		return false;
//	}
//
//	ParameterFactory paramFactory;
//
//	//We should clear out ourselves first, just in case this isn't a fresh
//	//container
//	parameters_.clear();
//
//	xmlStreamReader->readNext();
//	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Parameters") && !xmlStreamReader->atEnd())
//	{
//		if(!xmlStreamReader->isStartElement())
//		{
//			//Do nothing unless we're at a start element
//			xmlStreamReader->readNext();
//			continue;
//		}
//		QString name = xmlStreamReader->name().toString();
//		if(name == "Parameter")
//		{
//			//figure out the parameter type, and create a new instance of one
//			QString paramType = xmlStreamReader->attributes().value("type").toString();
//			QSharedPointer<Parameter> newParam = paramFactory.generateParameter(paramType);
//			if(newParam.isNull())
//			{
//				addError("ParameterContainer","Unrecognized parameter type",xmlStreamReader);			
//				return false;
//			}
//			if(!newParam->fromXml(xmlStreamReader))
//			{
//				addError("ParameterContainer","Failed to deserialize parameter",xmlStreamReader);			
//				return false;
//			}
//			parameters_.insert(newParam->name(),newParam);
//
//		}
//		else
//		{
//			addError("ParameterContainer", "Unexpected tag", xmlStreamReader);
//			return false;
//		}
//
//
//
//	}
//
//	if(xmlStreamReader->atEnd())
//	{
//		addError("ParameterContainer", "Unexpected end of document", xmlStreamReader);
//		return false;
//	}
//	return true;
//}

bool ParameterContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QList<QSharedPointer<Serializable>> newParams = getGeneratedChildren("Parameter");
	foreach(QSharedPointer<Serializable> newParam,newParams)
	{
		parameters_.insert(newParam.staticCast<Parameter>()->name(),newParam.staticCast<Parameter>());
	}
	return true;
}

}; //namespace Picto
