#include <QDebug>

#include "SwitchElement.h"
#include "../storage/AssetFactory.h"
#include "LogicResult.h"
#include "../memleakdetect.h"

namespace Picto
{

SwitchElement::SwitchElement()
{
	//AddDefinableProperty("Type","SwitchElement");	/*! \todo this shouldn't be a DEFINABLE property, but it needs to be here so that in StateMachine, element->type() gives the correct value.  Do something about this.*/
	AddDefinableProperty(QVariant::String,"Script","");
	
	defineResultFactoryType("",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LogicResult::Create))));
}

void SwitchElement::setScript(QString script)
{
	propertyContainer_->setPropertyValue("Script",script);
}

QSharedPointer<Asset> SwitchElement::Create()
{
	return QSharedPointer<Asset>(new SwitchElement());
}

QString SwitchElement::run(QSharedPointer<Engine::PictoEngine> engine)
{
	resetScriptableValues();
	runEntryScript();
	Property::startMonitoringForValueChange();
	QScriptValue returnVal = QScriptValue(false);
	runScript(getName().simplified().remove(' '),returnVal);
	QString resultString = returnVal.toString();
	runExitScript();
	if(getResult(resultString))
		return resultString;
	//If the string didn't match a result, the getReturnValueError function will return an error which
	//will come up in the debugger.  As far as this function goes, just go to the first result in the result list.
	Q_ASSERT(getResultList().length());
	return getResultList().first();
}

QString SwitchElement::slaveRun(QSharedPointer<Engine::PictoEngine> engine)
{
	QString result;
	return result; 
}

QString SwitchElement::getReturnValueError(QString scriptName,const QScriptValue& returnValue)
{
	if(scriptName != "Script")
		return "";

	QString resultError = "";
	if(Property::valueWasChanged())
		resultError = QString("The Script in Switch Element: \"%1\" caused the property value: \"%2\" to change.\n"
			"The Switch element Script should be used to select the result where contol flow needs to continue.\n"
			"Scripts that affect the experimental state should only be in Entry, Exit, or Frame Scripts.")
			.arg(getName())
			.arg(Property::changedValueName());

	if(getResult(returnValue.toString()).isNull())
		resultError = QString("The Script in Switch Element: \"%1\" returned a value: \"%2\" that was not in its results list.\n")
		.arg(getName())
		.arg(returnValue.toString());
	if(resultError.isEmpty())
	{
		resultError = StateMachineElement::getReturnValueError(scriptName,returnValue);
	}
	return resultError;
}

void SwitchElement::postDeserialize()
{
	StateMachineElement::postDeserialize();
	propertyContainer_->getProperty("UIEnabled")->setVisible(false);
}

bool SwitchElement::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!StateMachineElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

bool SwitchElement::hasScripts()
{
	if(StateMachineElement::hasScripts())
		return true;
	return (propertyContainer_->getPropertyValue("Script").toString() != "");
}

QMap<QString,QPair<QString,QString>>  SwitchElement::getScripts()
{
	QMap<QString,QPair<QString,QString>>  scripts = StateMachineElement::getScripts();
	if(!propertyContainer_->getPropertyValue("Script").toString().isEmpty())
		scripts[getName().simplified().remove(' ')] = QPair<QString,QString>(QString(),"Script");
	return scripts;
}


} //namespace Picto