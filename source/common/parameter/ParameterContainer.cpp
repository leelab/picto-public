#include "ParameterContainer.h"
#include "ParameterFactory.h"

namespace Picto {

ParameterContainer::ParameterContainer()
{
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


bool ParameterContainer::serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
{
	xmlStreamWriter->writeStartElement("Parameters");

	//! \TODO: Add code for outputting SessionData id values

	//serialize each parameter in the container
	foreach(QSharedPointer<Parameter> p, parameters_)
	{
		p->serializeAsXml(xmlStreamWriter);
	}

	xmlStreamWriter->writeEndElement(); //Parameters
	return true;
}

bool ParameterContainer::deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	//Do some basic error checking
	if(!xmlStreamReader->isStartElement())
		return false;
	if(xmlStreamReader->name() != "Parameters")
		return false;

	ParameterFactory paramFactory;

	//We should clear out ourselves first, just in case this isn't a fresh
	//container
	parameters_.clear();

	xmlStreamReader->readNext();
	while(!(xmlStreamReader->isEndElement() && xmlStreamReader->name().toString() == "Parameters") && !xmlStreamReader->atEnd())
	{
		if(!xmlStreamReader->isStartElement())
		{
			//Do nothing unless we're at a start element
			xmlStreamReader->readNext();
			continue;
		}
		QString name = xmlStreamReader->name().toString();
		if(name == "Parameter")
		{
			//figure out the parameter type, and create a new instance of one
			QString paramType = xmlStreamReader->attributes().value("type").toString();
			QSharedPointer<Parameter> newParam = paramFactory.generateParameter(paramType);
			if(!newParam->deserializeFromXml(xmlStreamReader))
				return false;
			parameters_.insert(newParam->name(),newParam);

		}
		else
		{
			//unexpected tag
			return false;
		}



	}
	return false;
}

}; //namespace Picto
