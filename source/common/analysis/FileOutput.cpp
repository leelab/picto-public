#include <QDir>
#include "FileOutput.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

#define CHARS_BEFORE_FLUSH 10000

FileOutput::FileOutput()
{
	AddDefinableProperty("FileName","");
	charsWritten_ = 0;
}

FileOutput::~FileOutput()
{
	//When this object is destroyed, remove its associated file from
	//the file system.
	if(file_->isOpen())
		file_->remove();
}

QSharedPointer<Asset> FileOutput::Create()
{
	return QSharedPointer<Asset>(new FileOutput());
}

void FileOutput::reset()
{	
	//If the output sub director for this file doesn't exist yet, make it.
	if(!QFile::exists(getTempOutputDir()+"/"+getOutputNamePrefix()))
	{	//The directory doesn't exist yet.  Make it.
		QDir dir;
		dir.mkdir(getTempOutputDir()+"/"+getOutputNamePrefix());
	}
	outputFileStream_.clear();
	QString fileName = getTempOutputDir()+"/"+getOutputNamePrefix()+"/"+getOutputNamePrefix()+propertyContainer_->getPropertyValue("FileName").toString();
	file_ = QSharedPointer<QFile>(new QFile(fileName));
	if(!file_->open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	outputFileStream_ = QSharedPointer<QTextStream>(new QTextStream(file_.data()));
	setValid(true);
}

QPointer<AnalysisOutputWidget> FileOutput::getOutputWidget()
{
	if(!file_)
		return QPointer<AnalysisOutputWidget>();
	QPointer<FileOutputWidget> outputWidget(new FileOutputWidget(1000));
	outputWidget->setFile(file_->fileName());
	return qobject_cast<AnalysisOutputWidget*>(outputWidget);
}

//bool FileOutput::saveOutputData(QString directory, QString filename)
//{
//	QString origName = propertyContainer_->getPropertyValue("FileName").toString();
//	QFile tempFile(getTempOutputDir()+"/"+origName);
//	if(!tempFile.open(QIODevice::ReadOnly | QIODevice::Text))
//	{
//		return false;
//	}
//	if(!QFile::exists(directory))
//	{	//The directory doesn't exist yet.  Make it.
//		QDir dir;
//		dir.mkdir(directory);
//	}
//	QString newFileName = directory + "/" + filename + origName.mid(origName.indexOf("."));
//	QFile::remove(newFileName);
//	bool result = tempFile.copy(newFileName);
//	tempFile.close();
//	return result;
//}

void FileOutput::finishUp()
{
	if(isValid())
	{
		file_->close();
	}
}

void FileOutput::writeLine(QString text)
{
	writeText(text + "\n");
}

void FileOutput::writeText(QString text)
{
	if(isValid())
	{
		charsWritten_ += text.size();
		(*outputFileStream_) << text;
		if(charsWritten_ > CHARS_BEFORE_FLUSH)
		{
			file_->flush();
			charsWritten_ = 0;
		}
	}
}

void FileOutput::postDeserialize()
{
	UIEnabled::postDeserialize();
	reset();
}

bool FileOutput::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!UIEnabled::validateObject(xmlStreamReader))
		return false;
	return true;
}