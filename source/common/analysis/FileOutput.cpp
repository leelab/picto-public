#include <QDir>
#include "FileOutput.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

QString FileOutput::outputDir_("");
int FileOutput::loadedObjects_(0);

FileOutput::FileOutput()
{
	initTempOutputDir();
	AddDefinableProperty("FileName","");
	outputWidget_ = NULL;
	loadedObjects_++;
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
{	if(outputWidget_)
	{
		outputWidget_->setParent(NULL);
		delete outputWidget_;
	}
	outputWidget_ = QPointer<QWidget>(new QTextEdit());
	qobject_cast<QTextEdit*>(outputWidget_)->setReadOnly(true);
	outputWidget_->setObjectName(propertyContainer_->getPropertyValue("FileName").toString());
	if(outputDir_.isEmpty())
		return;
	outputFileStream_.clear();
	file_ = QSharedPointer<QFile>(new QFile(getTempOutputDir()+"/"+propertyContainer_->getPropertyValue("FileName").toString()));
	if(!file_->open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	outputFileStream_ = QSharedPointer<QTextStream>(new QTextStream(file_.data()));
	setValid(true);
}

QPointer<QWidget> FileOutput::getOutputWidget()
{
	return outputWidget_;
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
		(*outputFileStream_) << text;
		qobject_cast<QTextEdit*>(outputWidget_)->moveCursor(QTextCursor::End);
		qobject_cast<QTextEdit*>(outputWidget_)->insertPlainText(text);
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
	dir.rmdir("AnalysisTool");
	dir.mkdir("AnalysisTool");
	dir.cd("AnalysisTool");
	outputDir_ = dir.absolutePath();
}