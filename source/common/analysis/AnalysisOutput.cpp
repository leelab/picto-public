#include "AnalysisOutput.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisOutput::AnalysisOutput()
{
}

AnalysisOutput::~AnalysisOutput()
{
}

void AnalysisOutput::postDeserialize()
{
	AnalysisTool::postDeserialize();
}

bool AnalysisOutput::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisTool::validateObject(xmlStreamReader))
		return false;
	return true;
}