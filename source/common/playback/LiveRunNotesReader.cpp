#include "LiveRunNotesReader.h"
using namespace Picto;

QString LiveRunNotesReader::getName()
{
	return "Sample Name";
}

double LiveRunNotesReader::getStartTime()
{
	return 0;
}

double LiveRunNotesReader::getEndTime()
{
	return -1;
}

QString LiveRunNotesReader::getNotes()
{
	return "Sample Notes";
}

int LiveRunNotesReader::getRunIndex()
{
	return 0;
}