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
}

QSharedPointer<Asset> FileOutput::Create()
{
	return QSharedPointer<Asset>(new FileOutput());
}

QPointer<AnalysisOutputWidget> FileOutput::getOutputWidget()
{
	if(!file_)
		return QPointer<AnalysisOutputWidget>();
	QPointer<FileOutputWidget> outputWidget(new FileOutputWidget(1000));
	outputWidget->setFile(file_->fileName());
	return qobject_cast<AnalysisOutputWidget*>(outputWidget);
}

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
		(*outputFileStream_).writeRawData(text.toAscii().constData(),text.size());
		if(charsWritten_ > CHARS_BEFORE_FLUSH)
		{
			file_->flush();
			charsWritten_ = 0;
		}
	}
}

void FileOutput::writeBinary(QString csvData,QString csvTypes)
{
	if(isValid())
	{
		QStringList data = csvData.split(",",QString::SkipEmptyParts);
		QStringList types = csvTypes.split(",",QString::SkipEmptyParts);
		QString currType = "short";
		for(int i=0;i<data.size();i++)
		{
			if(i<types.size())
			{
				currType = types[i].toLower();
			}
			if(currType == "short")
			{
				charsWritten_+=2;
				(*outputFileStream_) << data[i].toShort();
			}
			else if(currType == "int")
			{
				charsWritten_+=4;
				(*outputFileStream_) << data[i].toInt();
			}
			else if(currType == "long")
			{
				charsWritten_+=8;
				(*outputFileStream_) << data[i].toLong();
			}else if(currType == "float")
			{
				charsWritten_+=4;
				(*outputFileStream_) << data[i].toFloat();
			}
			else if(currType == "double")
			{
				charsWritten_+=8;
				(*outputFileStream_) << data[i].toDouble();
			}
		}

		if(charsWritten_ > CHARS_BEFORE_FLUSH)
		{
			file_->flush();
			charsWritten_ = 0;
		}
	}
}

void FileOutput::reset()
{	
	//If the output sub director for this file doesn't exist yet, make it.
	QString outputDir = getTempOutputDir();
	if(!QFile::exists(outputDir+"/"+getOutputNamePrefix()))
	{	//The directory doesn't exist yet.  Make it.
		QDir dir;
		dir.mkpath(outputDir+"/"+getOutputNamePrefix());
	}
	outputFileStream_.clear();
	QString fileName = outputDir+"/"+getOutputNamePrefix()+"/"+getOutputNamePrefix()+propertyContainer_->getPropertyValue("FileName").toString();
	file_ = QSharedPointer<QFile>(new QFile(fileName));
	if(!file_->open(QIODevice::WriteOnly | QIODevice::Text))
		return;
	outputFileStream_ = QSharedPointer<QDataStream>(new QDataStream(file_.data()));
	setValid(true);
}


void FileOutput::postDeserialize()
{
	AnalysisOutput::postDeserialize();
	reset();
}

bool FileOutput::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisOutput::validateObject(xmlStreamReader))
		return false;
	return true;
}