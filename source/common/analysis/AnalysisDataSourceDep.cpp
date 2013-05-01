#include "AnalysisDataSourceDep.h"
#include "AnalysisTrigger.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisDataSourceDep::AnalysisDataSourceDep()
{
	parentUsesSameIterator_ = false;
}

AnalysisDataSourceDep::~AnalysisDataSourceDep()
{
}

void AnalysisDataSourceDep::storeValue(QScriptValue triggerScript,const EventOrderIndex& index)
{
	triggerScript.setProperty(getName(),getValue(index)->scriptVal);
}

QSharedPointer<AnalysisValue> AnalysisDataSourceDep::getValue(const EventOrderIndex& index)
{
	if(parentUsesSameIterator_)
		return getParentAsset().staticCast<AnalysisTrigger>()->getLatestValue();
	if(dataIterator_.isNull())
	{
		QSharedPointer<AnalysisValue> returnVal(new AnalysisValue(qsEngine_,EventOrderIndex(index.time_)));
		fillOutScriptValue(returnVal);
		return returnVal;
	}
	QSharedPointer<AnalysisValue> prev = nextValue_;
	//Check if the last value we read last time is beyond the input time.
	//If so, nothing has changed, return the last value.
	if(prev && (prev->index > index))
		return latestValue_;
	
	//Get new values until the newest one is beyond the input
	//time or non-existant, then return the prior value.
	QSharedPointer<AnalysisValue> curr = dataIterator_->getNextValue();
	while(curr && (curr->index <= index) && (curr->index.isValid()))
	{
		prev = curr;
		curr = dataIterator_->getNextValue();
	}
	if(!curr || !curr->index.isValid())
	{
		curr = prev;
	}
	//Store the last read value change (which should be beyond the input time or empty)
	nextValue_ = curr;
	//Store the latest value
	latestValue_ = prev;
	return latestValue_;
}

QString AnalysisDataSourceDep::getIteratorDescriptor()
{
	if(!dataIterator_)
		return "";
	return QString(dataIterator_->metaObject()->className())+"-Desc-"+dataIterator_->propertyDescriptor();
}

void AnalysisDataSourceDep::fillOutScriptValue(QSharedPointer<AnalysisValue> val)
{
	Q_ASSERT_X(dataIterator_.isNull(),
	"AnalysisDataSourceDep::fillOutScriptValue",
	"Child classes must implement either createDataIterator() or fillOutScriptValue()");
};

QScriptValue AnalysisDataSourceDep::createScriptArray(unsigned int length)
{
	return qsEngine_->newArray(length);
}

void AnalysisDataSourceDep::loadSessionAndScriptTools(QSqlDatabase session,QSharedPointer<QScriptEngine> qsEngine,QScriptValue parent)
{
	session_ = session;
	qsEngine_ = qsEngine;
	parentScript_ = parent;

	dataIterator_.clear();
	parentUsesSameIterator_ = false;
	nextValue_.clear();
	if(!session_.isValid())
		return;
	dataIterator_=createDataIterator();
	QString myDesc = getIteratorDescriptor();
	QString parentDesc = getParentAsset().staticCast<AnalysisTrigger>()->getIteratorDescriptor();
	if((getIteratorDescriptor() == getParentAsset().staticCast<AnalysisTrigger>()->getIteratorDescriptor())
		&& !getIteratorDescriptor().isEmpty())
		parentUsesSameIterator_ = true;
}

void AnalysisDataSourceDep::setDataWindow(EventOrderIndex startFrom,EventOrderIndex endBefore)
{
	if(!dataIterator_ || parentUsesSameIterator_)
		return;
	dataIterator_->setDataWindow(startFrom,endBefore);
}

void AnalysisDataSourceDep::sessionDatabaseUpdated()
{
	if(!dataIterator_)
		return;
	dataIterator_->sessionDatabaseUpdated();
}

void AnalysisDataSourceDep::setScriptInfo(QString name,QScriptValue value)
{
	parentScript_.setProperty(name,value);
}

void AnalysisDataSourceDep::postDeserialize()
{
	UIEnabled::postDeserialize();
}

bool AnalysisDataSourceDep::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}