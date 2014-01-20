#include "ErrorList.h"
#include "../common/memleakdetect.h"

ErrorList::ErrorList(QWidget *parent)
: QWidget(parent)
{
}

/*! \brief Returns a list of all errors that were added using addError() since the last time that clear() was called.
 *	\details Returned errors are formated like the following:
 *	\code
		ErrorType: This is the error text.
	\endcode
 */
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

/*! \brief Clears all errors from this list.
 */
void ErrorList::clear()
{
	errorTypes_.clear();
	errorTexts_.clear();
}

/*! \brief Adds a new error to this Error list.
 *	@param errorType A string describing the type of error that occured.
 *	@param errorText A string describing what exactly the error was.
 *	\details The errorType and errorText will be formatted into a single 
 *	line when returned from getAllErrors().
 */
void ErrorList::addError(QString errorType, QString errorText)
{
	errorTypes_.append(errorType);
	errorTexts_.append(errorText);
}
