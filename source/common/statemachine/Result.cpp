#include "Result.h"
#include "../memleakdetect.h"
namespace Picto
{

Result::Result()
{
	//Add Optional Script property
	AddDefinableProperty("EntryScript","");
	resultScriptFactory_ = getAssetFactory("EntryScript");
	//By default, AddDefinableProperty sets minAssets to 1 and max to 1, reset min to 0 cause script
	//isn't required.
	//resultScriptFactory_->setMinAssets(0);
}

Result::Result(QString name)
{
	Q_ASSERT(false);	//Just put this here because I don't think we're using this constructor anymore and I want to get rid of it.

	//Add Optional Script property
	AddDefinableProperty("EntryScript","");
	resultScriptFactory_ = getAssetFactory("EntryScript");
	//By default, AddDefinableProperty sets minAssets to 1 and max to 1, reset min to 0 cause script
	//isn't required.
	//resultScriptFactory_->setMinAssets(0);

	initializePropertiesToDefaults();
	//Even though the name is edited, we set the result as unedited because we want 
	//this to serialize out with its name, but only if something else about it changes.
	setName(name);
	setUnedited();
}

QSharedPointer<Asset> Result::Create()
{
	return QSharedPointer<Asset>(new Result());
}

void Result::runResultScript()
{
	if(propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty() || propertyContainer_->getProperty("EntryScript")->isDeleted())
		return;
	QString scriptName = getName().simplified().remove(' ')+"EntryScript";
	runScript(scriptName);
}

void Result::postDeserialize()
{
	ScriptableContainer::postDeserialize();
}


bool Result::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ScriptableContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

bool Result::hasScripts()
{
	if(ScriptableContainer::hasScripts())
		return true;
	return !propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty() && !propertyContainer_->getProperty("EntryScript")->isDeleted();
}

QMap<QString,QPair<QString,QString>>  Result::getScripts()
{
	QMap<QString,QPair<QString,QString>>  scripts = ScriptableContainer::getScripts();
	if(!propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"EntryScript";
		scripts[scriptName] = QPair<QString,QString>(QString(),"EntryScript");
	}
	return scripts;
}

}//namespace Picto