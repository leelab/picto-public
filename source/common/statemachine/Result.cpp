#include "Result.h"
#include "../memleakdetect.h"
namespace Picto
{

Result::Result()
{
	turnOffUnusedAssetFactories();
	
	//Add Optional Script property
	AddDefinableProperty("Script","");
	resultScriptFactory_ = getAssetFactory("Script");
	//By default, AddDefinableProperty sets minAssets to 1 and max to 1, reset min to 0 cause script
	//isn't required.
	//resultScriptFactory_->setMinAssets(0);
}

Result::Result(QString name)
{
	Q_ASSERT(false);	//Just put this here because I don't think we're using this constructor anymore and I want to get rid of it.
	turnOffUnusedAssetFactories();

	//Add Optional Script property
	AddDefinableProperty("Script","");
	resultScriptFactory_ = getAssetFactory("Script");
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
	if(propertyContainer_->getPropertyValue("Script").toString().isEmpty() || propertyContainer_->getProperty("Script")->isDeleted())
		return;
	QString scriptName = getName().simplified().remove(' ')+"Script";
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
	return !propertyContainer_->getPropertyValue("Script").toString().isEmpty() && !propertyContainer_->getProperty("Script")->isDeleted();
}

QMap<QString,QPair<QString,QString>>  Result::getScripts()
{
	QMap<QString,QPair<QString,QString>>  scripts;
	if(!hasScripts())
		return scripts;

	if(!propertyContainer_->getPropertyValue("Script").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"Script";
		scripts[scriptName] = QPair<QString,QString>(QString(),"Script");
	}
	return scripts;
}

void Result::turnOffUnusedAssetFactories()
{
	scriptFunctionFactory_->setMaxAssets(0);
	parameterFactory_->setMaxAssets(0);
	visualElementFactory_->setMaxAssets(0);
	controlTargetFactory_->setMaxAssets(0);
	audioElementFactory_->setMaxAssets(0);
	outputSignalFactory_->setMaxAssets(0);
}

}//namespace Picto