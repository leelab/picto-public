#include "AnalysisVariable.h"
#include "AssociateExpLink.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisVariable::AnalysisVariable()
: Parameter()
{
	EXP_LINK_FACTORY_CREATION
}

void AnalysisVariable::reset()
{
	Parameter::reset();
}

bool AnalysisVariable::isPartOfSearch(SearchRequest searchRequest)
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

void AnalysisVariable::postDeserialize()
{
	Parameter::postDeserialize();
}

bool AnalysisVariable::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
