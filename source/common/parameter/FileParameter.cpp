#include <QFile>
#include <QRegularExpression>
#include "FileParameter.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Creates a new FileParameter.
 *	\details Adds a FileName string Property to hold the path to the file and a Data
 *	Property to hold the file's binary Data.  The "Data" Property is only used internally and 
 *	will be set invisible in postDeserialize().
 *	\note The file is defined by the binary data in "Data".  The FileName is used essentially
 *	for the file selection process.  When it changes, the FileParameter knows that it needs to
 *	load new data into Data.  If the file at the "FileName" path changes after that path was
 *	set to this FileParameter, it will not affect this object since the contents of "Data" define
 *	the file, and these are only set when the file is first selected.
 */
FileParameter::FileParameter()
: Parameter(),
updatingFile_(false)
{
	AddDefinableProperty(QVariant::String,"FileName",QVariant());
	AddDefinableProperty(QVariant::String,"Data",QVariant());
}

/*! \brief Returns a QByteArray containing the binary data that defines the file.
 *	\details If no file was set to this FileParameter, the returned QByteArray will
 *	be empty.
*/
QByteArray FileParameter::getFileData()
{
	return fileData_;
}

void FileParameter::postDeserialize()
{
	Parameter::postDeserialize();
	connect(propertyContainer_->getProperty("FileName").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(filePathChanged(Property*,QVariant)));
	connect(propertyContainer_->getProperty("Data").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(fileDataChanged(Property*,QVariant)));
	propertyContainer_->getProperty("Data")->setVisible(false);
	propertyContainer_->getProperty("UIEnabled")->setVisible(false);
	loadDataFromPropery();
}

/*! \brief Extends Parameter::validateObject() to verify that this Parameter's file data is not empty.
*/
bool FileParameter::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!Parameter::validateObject(xmlStreamReader))
		return false;
	if(propertyContainer_->getPropertyValue("Data").toString().isEmpty())
	{
		addError("FileParameter data file is empty.");
		return false;
	}
	return true;
}

/*! \brief Stores a QByteArray with file data based on the "Data" Property.
 *	\details We can't just used the "Data" Property directly since it contains
 *	binary data in Base64 (ie. converted into ASCII text that is useable in an XML
 *	file).  We need to convert it back to true Binary before storing it in our 
 *	QByteArray and using it.
 */
void FileParameter::loadDataFromPropery()
{
	QByteArray newFileData;
	QSharedPointer<Property> dataProp = propertyContainer_->getProperty("Data");
	fileData_ = QByteArray::fromBase64(dataProp->value().toByteArray());
	fileDataUpdated(fileData_);
}

/*! \brief Resets the data in the "Data" Property from file contents when the file path in the "FileName" Property
 *	changes.
 *	\details Called when the "FileName" Property changes.
*/
void FileParameter::filePathChanged(Property*,QVariant val)
{
	//Reentrant
	if(updatingFile_)
		return;
	updatingFile_ = true;
	QString newFilePath = val.toString();
	

	if(!QFile::exists(newFilePath))
	{	//File doesn't exist
		propertyContainer_->setPropertyValue("Data","");
		updatingFile_ = false;
		return;
	}
	QFile newFile(newFilePath);

	//Open file
	if(!newFile.open(QIODevice::ReadOnly))
	{
		propertyContainer_->setPropertyValue("Data","");
		updatingFile_ = false;
		return;
	}
	QByteArray newFileData = newFile.readAll();
	propertyContainer_->setPropertyValue("Data",newFileData.toBase64());
	newFile.close();

	//prop->setValue("");	//Set the file path value to empty so that if someone reloads the same file, a property
	//					//change will be detected and this function will be able to load the file binary 
	updatingFile_ = false;
}

/*! \brief Called when the "Data" Property changes.  Calls loadDataFromProperty() so that child classes will
 *	know about the change too.
 */
void FileParameter::fileDataChanged(Property*,QVariant)
{
	loadDataFromPropery();
}

}; //namespace Picto
