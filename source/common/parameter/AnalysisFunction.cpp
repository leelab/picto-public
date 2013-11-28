#include "AnalysisFunction.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisFunction::AnalysisFunction()
{
	EXP_LINK_FACTORY_CREATION	
}
/*! \brief Creates a new AnalysisFunction and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> AnalysisFunction::Create()
{
	return QSharedPointer<Asset>(new AnalysisFunction());
}

bool AnalysisFunction::isPartOfSearch(SearchRequest searchRequest)
{
	//If the search is for all analyses
	//OR If the search is for active analyses and this is an active analysis,
	//OR If the search is for inactive analyses and this is an inactive analysis,
	if	(	(searchRequest.group == SearchRequest::ALL_ANALYSES)	
			||	((searchRequest.group & SearchRequest::ACTIVE_ANALYSES) && getDesignConfig()->isAnalysisActive(getAssociateId()))
			||	((searchRequest.group & SearchRequest::INACTIVE_ANALYSES) && !getDesignConfig()->isAnalysisActive(getAssociateId()))
		)
		return true;
	return false;
}

void AnalysisFunction::postDeserialize()
{
	ScriptFunction::postDeserialize();
}

bool AnalysisFunction::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ScriptFunction::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
