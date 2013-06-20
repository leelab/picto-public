#include <QDir>
#include <QMessageBox>
#include "AnalysisFileOutput.h"
#include "AnalysisFileOutputWidget.h"
#include "../memleakdetect.h"

namespace Picto {

#define CHARS_BEFORE_FLUSH 10000

AnalysisFileOutput::AnalysisFileOutput()
: AnalysisOutput()
{
	AddDefinableProperty("FileSuffix","");
	typeList_<< "Text" << "BigEndian" << "LittleEndian";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"FileType",0,"enumNames",typeList_);

	charsWritten_ = 0;
	setValid(false);
}

QSharedPointer<Asset> AnalysisFileOutput::Create()
{
	return QSharedPointer<Asset>(new AnalysisFileOutput());
}

//QPointer<AnalysisOutputWidget> AnalysisFileOutput::getOutputWidget()
//{
//	if(!file_ || !isValid())
//		return QPointer<AnalysisOutputWidget>();
//	QPointer<FileOutputWidget> outputWidget(new FileOutputWidget(1000));
//	outputWidget->setFile(file_->fileName());
//	return qobject_cast<AnalysisOutputWidget*>(outputWidget);
//}

void AnalysisFileOutput::reset()
{	
	AnalysisOutput::reset();
}

void AnalysisFileOutput::writeLine(QString text)
{
	writeText(text + "\n");
}

void AnalysisFileOutput::writeText(QString text)
{
	if(!isValid())
	{
		openFile();
	}
	charsWritten_ += text.size();
	(*outputFileStream_).writeRawData(text.toLatin1().constData(),text.size());
	if(charsWritten_ > CHARS_BEFORE_FLUSH)
	{
		file_->flush();
		charsWritten_ = 0;
	}
}

void AnalysisFileOutput::writeBinary(QString csvData,QString csvTypes)
{
	if(!isValid())
	{
		openFile();
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

void AnalysisFileOutput::postDeserialize()
{
	AnalysisOutput::postDeserialize();
}

bool AnalysisFileOutput::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisOutput::validateObject(xmlStreamReader))
		return false;
	return true;
}

AnalysisOutputWidget* AnalysisFileOutput::createWidget()
{
	if(!isValid())
	{
		openFile();
	}
	//Now that the file is opened, add a widget showing it to this run's container widget
	AnalysisFileOutputWidget* outputWidget(new AnalysisFileOutputWidget(1000));
	outputWidget->setFile(file_->fileName());
	return outputWidget;
}

void AnalysisFileOutput::finishUp()
{
	if(!file_.isNull() && file_->isOpen())
	{
		file_->close();
	}
	charsWritten_ = 0;
	setValid(false);
}

//If the file is opened in text mode, it will convert all binary 0x0A to 0x0D0A
//we need to make sure its opened in the right mode.
void AnalysisFileOutput::openFile()
{
	//If the output sub director for this file doesn't exist yet, make it.
	QString outputDir = getTempOutputDir();
	if(!QFile::exists(outputDir+"/"+getRunName()))
	{	//The directory doesn't exist yet.  Make it.
		QDir dir;
		dir.mkpath(outputDir+"/"+getRunName());
	}
	outputFileStream_.clear();
	QString fileName = outputDir+"/"+getRunName()+"/"+getRunName()+propertyContainer_->getPropertyValue("FileSuffix").toString();
	file_ = QSharedPointer<QFile>(new QFile(fileName));

	//Open file
	QString fileType = typeList_.value(propertyContainer_->getPropertyValue("FileType").toInt(),"");
	bool textMode = (fileType == "Text");
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
		if(typeList_.value(propertyContainer_->getPropertyValue("FileType").toInt(),"") == "BigEndian")
			outputFileStream_->setByteOrder(QDataStream::BigEndian);
		else if(typeList_.value(propertyContainer_->getPropertyValue("FileType").toInt(),"") == "LittleEndian")
			outputFileStream_->setByteOrder(QDataStream::LittleEndian);
		else
			Q_ASSERT(false);
	}
	charsWritten_ = 0;
	setValid(true);
}

}; //namespace Picto
