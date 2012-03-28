#include "AnalysisTool.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

AnalysisTool::AnalysisTool()
{
	setValid(false);
}

AnalysisTool::~AnalysisTool()
{
}

void AnalysisTool::postDeserialize()
{
	Scriptable::postDeserialize();
}

bool AnalysisTool::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Scriptable::validateObject(xmlStreamReader))
		return false;
	return true;
}