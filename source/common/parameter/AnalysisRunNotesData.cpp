#include "AnalysisRunNotesData.h"
#include "../memleakdetect.h"

namespace Picto
{
AnalysisRunNotesData::AnalysisRunNotesData()
: 
  AnalysisDataSource()
{
}

QSharedPointer<Asset> AnalysisRunNotesData::Create()
{
	return QSharedPointer<Asset>(new AnalysisRunNotesData());
}

void AnalysisRunNotesData::enteredScope()
{
	AnalysisDataSource::enteredScope();
}

QString AnalysisRunNotesData::getRunName()
{
	Q_ASSERT(!getDesignConfig()->getRunNotesReader().isNull());
	if(!getDesignConfig()->getRunNotesReader())
		return "";
	return getDesignConfig()->getRunNotesReader()->getName();
}

double AnalysisRunNotesData::getGlobalStartTime()
{
	Q_ASSERT(!getDesignConfig()->getRunNotesReader().isNull());
	if(!getDesignConfig()->getRunNotesReader())
		return -1;
	return getDesignConfig()->getRunNotesReader()->getStartTime();
}

double AnalysisRunNotesData::getGlobalEndTime()
{
	Q_ASSERT(!getDesignConfig()->getRunNotesReader().isNull());
	if(!getDesignConfig()->getRunNotesReader())
		return -1;
	return getDesignConfig()->getRunNotesReader()->getEndTime();
}

QString AnalysisRunNotesData::getRunNotes()
{
	Q_ASSERT(!getDesignConfig()->getRunNotesReader().isNull());
	if(!getDesignConfig()->getRunNotesReader())
		return "";
	QString result = getDesignConfig()->getRunNotesReader()->getNotes();
	return result;
}

int AnalysisRunNotesData::getRunIndex()
{
	Q_ASSERT(!getDesignConfig()->getRunNotesReader().isNull());
	if(!getDesignConfig()->getRunNotesReader())
		return -1;
	return getDesignConfig()->getRunNotesReader()->getRunIndex();
}

void AnalysisRunNotesData::postDeserialize()
{
	AnalysisDataSource::postDeserialize();
}

bool AnalysisRunNotesData::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisDataSource::validateObject(xmlStreamReader))
		return false;
	return true;
}

} //namespace Picto