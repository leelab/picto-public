#include "ErrorList.h"

ErrorList::ErrorList(QWidget *parent)
: QWidget(parent)
{
}

QStringList ErrorList::getAllErrors()
{
	QStringList errorList;
	for(int i=0; i<errorTypes_.length(); i++)
	{
		QString errStr = errorTypes_[i] + ": "+errorTexts_[i];
		errorList.append(errStr);
	}
	return errorList;
}

void ErrorList::clear()
{
	errorTypes_.clear();
	errorTexts_.clear();
}

void ErrorList::addError(QString errorType, QString errorText)
{
	errorTypes_.append(errorType);
	errorTexts_.append(errorText);
}
