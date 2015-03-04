#include "Result.h"
#include "../memleakdetect.h"
namespace Picto
{

/*! \brief Constructs a new result.
 *	\details An optional EntryScript Property defining a Script that will be called when control flow reaches this Result 
 *	is added in this constructor.
 */
Result::Result()
{
	//Add Optional Script property
	AddDefinableProperty("EntryScript","");
}

/*! \brief Creates a new Result object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> Result::Create()
{
	return QSharedPointer<Asset>(new Result());
}

/*! \brief Runs this Result's ResultScript if it is not empty.*/
void Result::runResultScript()
{
	if(propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty() || propertyContainer_->getProperty("EntryScript")->isDeleted())
		return;
	QString scriptName = getName().simplified().remove(' ')+"EntryScript";
	runScript(scriptName);
}

/*! \brief Extends ScriptableContainer::postDeserialize() to set the UIEnabled Property invisible since it doesn't do anything in Result objects.
 */
void Result::postDeserialize()
{
	ScriptableContainer::postDeserialize();
	propertyContainer_->getProperty("UIEnabled")->setVisible(false);
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

QMap<QString,QString>  Result::getScripts()
{
	QMap<QString,QString>  scripts = ScriptableContainer::getScripts();
	if(!propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"EntryScript";
		scripts[scriptName] = QString("EntryScript");
	}
	return scripts;
}

}//namespace Picto