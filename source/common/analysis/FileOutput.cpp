#include <QDir>
#include "FileOutput.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

#define CHARS_BEFORE_FLUSH 10000

QString FileOutput::outputDir_("");
int FileOutput::loadedObjects_(0);

FileOutput::FileOutput()
{
	initTempOutputDir();
	AddDefinableProperty("FileName","");
	outputWidget_ = QPointer<FileOutputWidget>(new FileOutputWidget(1000));
	loadedObjects_++;
	charsWritten_ = 0;
}

FileOutput::~FileOutput()
{
	//When this object is destroyed, remove its associated file from
	//the file system.
	if(file_->isOpen())
		file_->remove();
	loadedObjects_--;
	//If their are no more FileOutput objects loaded and the AnalysisTool 
	//directory is empty, remove it.
	if(!loadedObjects_)
	{
		QDir dir(QDir::current());
		dir.cd("AnalysisTool");
		dir.cdUp();
		dir.rmdir("AnalysisTool");
		outputDir_ = "";
	}
}

QSharedPointer<Asset> FileOutput::Create()
{
	return QSharedPointer<Asset>(new FileOutput());
}

void FileOutput::reset()
{	
	if(outputDir_.isEmpty())
		return;
	outputFileStream_.clear();
	QString fileName = getTempOutputDir()+"/"+propertyContainer_->getPropertyValue("FileName").toString();
	file_ = QSharedPointer<QFile>(new QFile(fileName));
	if(!file_->open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	outputFileStream_ = QSharedPointer<QTextStream>(new QTextStream(file_.data()));
	outputWidget_->setFile(fileName);
	setValid(true);
}

QPointer<QWidget> FileOutput::getOutputWidget()
{
	return qobject_cast<QWidget*>(outputWidget_);
}

bool FileOutput::saveOutputData(QString directory, QString filename)
{
	QString origName = propertyContainer_->getPropertyValue("FileName").toString();
	QFile tempFile(getTempOutputDir()+"/"+origName);
	if(!tempFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return false;
	}
	if(!QFile::exists(directory))
	{	//The directory doesn't exist yet.  Make it.
		QDir dir;
		dir.mkdir(directory);
	}
	QString newFileName = directory + "/" + filename + origName.mid(origName.indexOf("."));
	QFile::remove(newFileName);
	bool result = tempFile.copy(newFileName);
	tempFile.close();
	return result;
}

void FileOutput::finishUp()
{
	if(isValid())
	{
		file_->close();
		outputWidget_->refreshContents();
	}
}

QString FileOutput::getTempOutputDir()
{
	return outputDir_;
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

//Will create an empty AnalysisTool directory in the runpath once
//per executable run, clearing out any previous contents if there 
//were any.
void FileOutput::initTempOutputDir()
{
	if(!outputDir_.isEmpty())
		return;
	QDir dir(QDir::current());
	dir.rmdir("FileOutputTool");
	dir.mkdir("FileOutputTool");
	dir.cd("FileOutputTool");
	outputDir_ = dir.absolutePath();
}