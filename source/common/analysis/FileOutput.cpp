#include <QDir>
#include <QMessageBox>
#include "FileOutput.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

#define CHARS_BEFORE_FLUSH 10000

FileOutput::FileOutput()
{
	AddDefinableProperty("FileName","");
	endianList_ << "BigEndian" << "LittleEndian";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"BinaryByteOrder",0,"enumNames",endianList_);

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
	if(!file_ || !isValid())
		return QPointer<AnalysisOutputWidget>();
	QPointer<FileOutputWidget> outputWidget(new FileOutputWidget(1000));
	outputWidget->setFile(file_->fileName());
	return qobject_cast<AnalysisOutputWidget*>(outputWidget);
}

void FileOutput::finishUp()
{
	file_->close();
}

void FileOutput::writeLine(QString text)
{
	writeText(text + "\n");
}

void FileOutput::writeText(QString text)
{
	if(!isValid())
	{
		openFile(true);
	}
	charsWritten_ += text.size();
	(*outputFileStream_).writeRawData(text.toLatin1().constData(),text.size());
	if(charsWritten_ > CHARS_BEFORE_FLUSH)
	{
		file_->flush();
		charsWritten_ = 0;
	}
}

void FileOutput::writeBinary(QString csvData,QString csvTypes)
{
	if(!isValid())
	{
		openFile(false);
	}
	QStringList data = csvData.split(",");
	QStringList types = csvTypes.split(",");
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
			(*outputFileStream_) << short(data[i].toDouble());
		}
		else if(currType == "int")
		{
			charsWritten_+=4;
			(*outputFileStream_) << int(data[i].toDouble());
		}
		else if(currType == "long")
		{
			charsWritten_+=8;
			(*outputFileStream_) << long(data[i].toDouble());
		}else if(currType == "float")
		{
			charsWritten_+=4;
			(*outputFileStream_) << float(data[i].toDouble());
		}
		else if(currType == "double")
		{
			charsWritten_+=8;
			(*outputFileStream_) << double(data[i].toDouble());
		}
	}

	if(charsWritten_ > CHARS_BEFORE_FLUSH)
	{
		file_->flush();
		charsWritten_ = 0;
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
	setValid(false);
	file_ = QSharedPointer<QFile>(new QFile(fileName));
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

//If the file is opened in text mode, it will convert all binary 0x0A to 0x0D0A
//we need to make sure its opened in the right mode.
void FileOutput::openFile(bool textMode)
{
	if(!file_->open(textMode?(QIODevice::WriteOnly | QIODevice::Text):QIODevice::WriteOnly))
	{
		QMessageBox error;
		error.setText("Could not open file");
		error.setDetailedText("Could not open: " + file_->fileName());
		error.setIcon(QMessageBox::Critical);
		error.exec();
		return;
	}
	outputFileStream_ = QSharedPointer<QDataStream>(new QDataStream(file_.data()));
	if(!textMode)
	{
		outputFileStream_->setFloatingPointPrecision(QDataStream::SinglePrecision);
		if(endianList_.value(propertyContainer_->getPropertyValue("BinaryByteOrder").toInt(),"") == "BigEndian")
			outputFileStream_->setByteOrder(QDataStream::BigEndian);
		else if(endianList_.value(propertyContainer_->getPropertyValue("BinaryByteOrder").toInt(),"") == "LittleEndian")
			outputFileStream_->setByteOrder(QDataStream::LittleEndian);
		else
			Q_ASSERT(false);
	}
	setValid(true);
}