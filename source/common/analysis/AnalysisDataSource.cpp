#include "AnalysisDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisDataSource::AnalysisDataSource()
{
	usefulScriptValues_ = 0;
}

AnalysisDataSource::~AnalysisDataSource()
{
}

void AnalysisDataSource::clearValuesTo(const EventOrderIndex& index)
{
	QLinkedList<QSharedPointer<TriggeredAnalysisValue>>::iterator iter = values_.begin();
	while(iter != values_.end() && (*iter)->triggeredIndex < index)
		iter++;
	values_.erase(values_.begin(),iter);
	usefulScriptValues_ = 0;
}

void AnalysisDataSource::storeValue(const EventOrderIndex& index)
{
	QSharedPointer<TriggeredAnalysisValue> newVal(new TriggeredAnalysisValue(index,getValue(index)));
	values_.append(newVal);
}

void AnalysisDataSource::prepareValuesForScript(const EventOrderIndex& beforeIndex)
{
	scriptValues_.resize(values_.size());
	int i=0;
	foreach(QSharedPointer<TriggeredAnalysisValue> value,values_)
	{
		if(value->triggeredIndex >= beforeIndex)
			break;
		scriptValues_[i++] = value;
	}
	if(i<scriptValues_.size())
		usefulScriptValues_ = i;
	else
		usefulScriptValues_ = scriptValues_.size();
}

void AnalysisDataSource::sessionDatabaseUpdated()
{
	recheckSessionData();
}

QSharedPointer<AnalysisValue> AnalysisDataSource::getScriptValue(int index)
{
	Q_ASSERT_X(index <= usefulScriptValues_,"AnalysisDataSource::getScriptValue"
		,QString("The index: %1 was requested from the %2 DataSource which has only %3 values available.")
		.arg(index).arg(getName()).arg(usefulScriptValues_).toAscii());
	return scriptValues_[index]->value;
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