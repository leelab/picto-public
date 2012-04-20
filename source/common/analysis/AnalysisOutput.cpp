#include <QDir>
#include "AnalysisOutput.h"

#include "../../common/memleakdetect.h"
using namespace Picto;

QString AnalysisOutput::outputDir_("");
int AnalysisOutput::loadedObjects_(0);
AnalysisOutput::AnalysisOutput()
{
	loadedObjects_++;
	initTempOutputDir();
	prefix_ = "untitled";
}

AnalysisOutput::~AnalysisOutput()
{
	loadedObjects_--;
	//If their are no more AnalysisOutput objects loaded, remove the AnalysisOutput directory.
	if(!loadedObjects_)
	{
		QDir dir(QDir::current());
		dir.cd("AnalysisOutput");
		dir.cdUp();
		dir.rmdir("AnalysisOutput");
		outputDir_ = "";
	}
}

QString AnalysisOutput::getTempOutputDir()
{
	return outputDir_;
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

//Will create an empty AnalysisOutput directory in the runpath once
//per executable run, clearing out any previous contents if there 
//were any.
void AnalysisOutput::initTempOutputDir()
{
	if(!outputDir_.isEmpty())
		return;
	QDir dir(QDir::current());
	dir.rmdir("AnalysisOutput");
	dir.mkdir("AnalysisOutput");
	dir.cd("AnalysisOutput");
	outputDir_ = dir.absolutePath();
}