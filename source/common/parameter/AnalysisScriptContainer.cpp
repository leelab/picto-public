#include "AnalysisScriptContainer.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisScriptContainer::AnalysisScriptContainer()
: ScriptableContainer()
{
	EXP_LINK_FACTORY_CREATION
	AddDefinableProperty(QVariant::String,"AnalysisEntryScript","");
	//Depending whether this is attached to a state or result, there may not be any
	//need for AnalysisFrameScript or AnalysisExitScript
	AddDefinableProperty(QVariant::String,"AnalysisFrameScript","",QMap<QString,QVariant>(),0,1);
	AddDefinableProperty(QVariant::String,"AnalysisExitScript","",QMap<QString,QVariant>(),0,1);
}

QSharedPointer<Asset> AnalysisScriptContainer::Create()
{
	return QSharedPointer<Asset>(new AnalysisScriptContainer());
}

void AnalysisScriptContainer::runScript(ScriptType type)
{
	switch(type)
	{
	case ENTRY:
		ScriptableContainer::runScript("AnalysisEntryScript");
		break;
	case FRAME:
		if(propertyContainer_->getProperty("AnalysisFrameScript"))
			ScriptableContainer::runScript("AnalysisFrameScript");
		break;
	case EXIT:
		if(propertyContainer_->getProperty("AnalysisExitScript"))
			ScriptableContainer::runScript("AnalysisExitScript");
		break;
	}
}

//Returns true if this object contains a property of the input type, whether or not it is empty.
bool AnalysisScriptContainer::hasScriptPropertyType(ScriptType type)
{
	switch(type)
	{
	case ENTRY:
		return true;
		break;
	case FRAME:
		return propertyContainer_->getProperty("AnalysisFrameScript");
		break;
	case EXIT:
		return propertyContainer_->getProperty("AnalysisExitScript");
		break;
	}
	return false;
}

void AnalysisScriptContainer::postDeserialize()
{
	ScriptableContainer::postDeserialize();
}

bool AnalysisScriptContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ScriptableContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

bool AnalysisScriptContainer::hasScripts()
{
	return	!propertyContainer_->getPropertyValue("AnalysisEntryScript").toString().isEmpty() 
			||	(hasScriptPropertyType(FRAME)
				&& !propertyContainer_->getPropertyValue("AnalysisFrameScript").toString().isEmpty())
			|| (hasScriptPropertyType(EXIT) 
				&& !propertyContainer_->getPropertyValue("AnalysisExitScript").toString().isEmpty());
}

QMap<QString,QPair<QString,QString>> AnalysisScriptContainer::getScripts()
{
	QMap<QString,QPair<QString,QString>>  scripts = ScriptableContainer::getScripts();
	QString script = propertyContainer_->getPropertyValue("AnalysisEntryScript").toString();
	if(!script.isEmpty())
	{
		QString scriptName = getName();
		scripts[scriptName] = QPair<QString,QString>(QString(),"AnalysisEntryScript");
	}
	if(hasScriptPropertyType(FRAME))
	{
		QString script = propertyContainer_->getPropertyValue("AnalysisFrameScript").toString();
		if(!script.isEmpty())
		{
			QString scriptName = getName();
			scripts[scriptName] = QPair<QString,QString>(QString(),"AnalysisFrameScript");
		}
	}
	if(hasScriptPropertyType(EXIT))
	{
		QString script = propertyContainer_->getPropertyValue("AnalysisExitScript").toString();
		if(!script.isEmpty())
		{
			QString scriptName = getName();
			scripts[scriptName] = QPair<QString,QString>(QString(),"AnalysisExitScript");
		}
	}
	return scripts;
}
}; //namespace Picto
