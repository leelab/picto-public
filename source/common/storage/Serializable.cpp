#include "Serializable.h"

using namespace Picto;

QStringList Serializable::errors_;

Serializable::Serializable()
//: QObject(NULL),
//isNew_(false),	//!!!!!!!!THIS NEEDS TO BE INITIALIZED TO TRUE.  I JUST CHANGED IT FOR DEBUGGING PURPOSES!!!!!!!!!!!!!
//edited_(false),
//deleted_(false)
{
//	//connect(this, SIGNAL(edited()), this, SLOT(receivedEditedSignal()));
//	//connect(this, SIGNAL(deleted()), this, SLOT(receivedDeletedSignal()));
}

//bool Serializable::toXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter)
//{
//	if(isDeleted())
//		return true;
//	return serializeAsXml(xmlStreamWriter);
//}
//bool Serializable::fromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader)
//{
//	bool returnVal = deserializeFromXml(xmlStreamReader);
//	edited_ = false;
//	isNew_ = false;
//	deleted_ = false;
//	return returnVal;
//}

//void Serializable::setDeleted()
//{
//	emit deleted();
//}
//
//void Serializable::receivedEditedSignal()
//{
//	edited_ = true;
//}
//void Serializable::receivedDeletedSignal()
//{
//	deleted_ = true;
//}

void Serializable::AddError(QString objectType, QString errorMsg, QSharedPointer<QXmlStreamReader> xmlStreamReader, QSharedPointer<QStringList> errors)
{
	QString newErr = "ERROR\n";
	newErr += "Object: " + objectType + "\n";
	newErr += QString("Line: %1\n").arg(xmlStreamReader->lineNumber());
	newErr += "Name: " + xmlStreamReader->name().toString() + "\n";
	newErr += "Element: " + xmlStreamReader->tokenString() + "\n";
	newErr += "Message: " + errorMsg + "\n";
	errors->push_back(newErr);
}

/*! \brief Adds an error message to the list of errors
 *
 *	We maintain a list of errors so that errors can be tracked through the deserialization
 *	process.
 */
void Serializable::addError(QString objectType, QString errorMsg, QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	QString newErr = "ERROR\n";
	newErr += "Object: " + objectType + "\n";
	newErr += QString("Line: %1\n").arg(xmlStreamReader->lineNumber());
	newErr += "Name: " + xmlStreamReader->name().toString() + "\n";
	newErr += "Element: " + xmlStreamReader->tokenString() + "\n";
	newErr += "Message: " + errorMsg + "\n";

	errors_.append(newErr);
}

void Serializable::addError(QString objectType, QString errorMsg)
{
	QString newErr = "ERROR\n";
	newErr += "Object: " + objectType + "\n";
	newErr += "Message: " + errorMsg + "\n";

	errors_.append(newErr);
}


/*!	\brief Returns a string listing all errors that have occured
 */
QString Serializable::getErrors()
{
	QString errorString = "DESERIALIZATION ERRORS\n\n";

	for(int i=0; i<errors_.length(); i++)
	{
		errorString += errors_[i] + "\n";
	}
	return errorString;

}
