#include "SignalDataSource.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

SignalDataSource::SignalDataSource()
{
	AddDefinableProperty("SignalName","");
}

SignalDataSource::~SignalDataSource()
{
}

QSharedPointer<Asset> SignalDataSource::Create()
{
	return QSharedPointer<Asset>(new SignalDataSource());
}

QSharedPointer<AnalysisDataIterator> SignalDataSource::createDataIterator()
{
	QSharedPointer<SignalDataIterator>returnVal(
							new SignalDataIterator(qsEngine_,session_,
								propertyContainer_->getPropertyValue("SignalName").toString())
							);
	int numSubChans = returnVal->numSubChannels();
	setScriptInfo("numSubChannels",numSubChans);
	QScriptValue subChanNames = createScriptArray(numSubChans);
	for(int i=0;i<numSubChans;i++)
		subChanNames.setProperty(i,returnVal->subChanName(i));
	setScriptInfo("subChannelName",subChanNames);
	return returnVal;
}

void SignalDataSource::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool SignalDataSource::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}