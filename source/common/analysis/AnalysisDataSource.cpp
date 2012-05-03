#include "AnalysisDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisDataSource::AnalysisDataSource()
{
}

AnalysisDataSource::~AnalysisDataSource()
{
}

void AnalysisDataSource::storeValue(QScriptValue triggerScript,const EventOrderIndex& index)
{
	triggerScript.setProperty(getName(),getValue(index)->scriptVal);
}

void AnalysisDataSource::setScriptObjects(QSharedPointer<QScriptEngine> qsEngine,QScriptValue parent)
{
	qsEngine_ = qsEngine;
	parentScript_ = parent;
}

void AnalysisDataSource::sessionDatabaseUpdated()
{
	recheckSessionData();
}

void AnalysisDataSource::setScriptInfo(QString name,QScriptValue value)
{
	parentScript_.setProperty(name,value);
}

void AnalysisDataSource::postDeserialize()
{
	UIEnabled::postDeserialize();
}

bool AnalysisDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}