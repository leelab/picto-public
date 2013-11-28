#include "AnalysisRunNotesData.h"
#include "../memleakdetect.h"

namespace Picto
{
AnalysisRunNotesData::AnalysisRunNotesData()
: 
  AnalysisDataSource()
{
}

/*! \brief Creates a new AnalysisRunNotesData object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> AnalysisRunNotesData::Create()
{
	return QSharedPointer<Asset>(new AnalysisRunNotesData());
}

void AnalysisRunNotesData::enteredScope()
{
	AnalysisDataSource::enteredScope();
}

/*!	\brief Gets the name of the current run.
 *	\details This name is either a default one based on the start time of the run, 
 *	or the one set by the operator in the run info section during a session.
 */
QString AnalysisRunNotesData::getRunName()
{
	Q_ASSERT(!getDesignConfig()->getRunNotesReader().isNull());
	if(!getDesignConfig()->getRunNotesReader())
		return "";
	return getDesignConfig()->getRunNotesReader()->getName();
}

/*! \brief Returns the time that this run started in seconds where time zero is the 
 *	beginning of the session.
 */
double AnalysisRunNotesData::getGlobalStartTime()
{
	Q_ASSERT(!getDesignConfig()->getRunNotesReader().isNull());
	if(!getDesignConfig()->getRunNotesReader())
		return -1;
	return getDesignConfig()->getRunNotesReader()->getStartTime();
}

/*! \brief Returns the time that this run ends in seconds where time zero is the 
 *	beginning of the session.
 */
double AnalysisRunNotesData::getGlobalEndTime()
{
	Q_ASSERT(!getDesignConfig()->getRunNotesReader().isNull());
	if(!getDesignConfig()->getRunNotesReader())
		return -1;
	return getDesignConfig()->getRunNotesReader()->getEndTime();
}

/*! \brief Returns the notes string that was saved by the operator during 
 *	the session.  If no notes were saved, an empty string is returned.
 */
QString AnalysisRunNotesData::getRunNotes()
{
	Q_ASSERT(!getDesignConfig()->getRunNotesReader().isNull());
	if(!getDesignConfig()->getRunNotesReader())
		return "";
	QString result = getDesignConfig()->getRunNotesReader()->getNotes();
	return result;
}
/*! \brief Returns the index of this run in this sessions list of task runs, 
 *	where the first run has an index of zero.
 */
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