#include <QFile>
#include <QRegularExpression>
#include "FileParameter.h"
#include "../memleakdetect.h"

namespace Picto {

FileParameter::FileParameter()
: Parameter(),
updatingFile_(false)
{
	AddDefinableProperty(QVariant::String,"FileName",QVariant());
	AddDefinableProperty(QVariant::String,"Data",QVariant());
}


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

void FileParameter::loadDataFromPropery()
{
	QByteArray newFileData;
	QSharedPointer<Property> dataProp = propertyContainer_->getProperty("Data");
	fileData_ = QByteArray::fromBase64(dataProp->value().toByteArray());
	fileDataUpdated(fileData_);
}

void FileParameter::filePathChanged(Property* prop,QVariant val)
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

void FileParameter::fileDataChanged(Property*,QVariant)
{
	loadDataFromPropery();
}

}; //namespace Picto
