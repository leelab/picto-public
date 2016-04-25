#include <QDir>
#include <QMessageBox>
#include "AnalysisFileOutput.h"
#include "AnalysisFileOutputWidget.h"
#include "AnalysisBinaryOutputWidget.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \file */
/*! \brief The number of characters that must be written to the file before the file buffer is 
 *	automatically flushed to disk.*/
#define CHARS_BEFORE_FLUSH 10000

/*! \brief Creates a new AnalysisFileOutput object.
 *	\details 
 *		- Adds a FileSuffix property to hold the string that will be appended to the run name
 *	to create the name of the file underlying this object, (ie .txt).
 *		- Adds a FileType list property to hold the type of the text file, Text, Big Endian binary,
 * or Little Endiant binary.
 */
AnalysisFileOutput::AnalysisFileOutput()
: AnalysisOutput()
{
	AddDefinableProperty("FileSuffix","");
	typeList_<< "Text" << "BigEndian" << "LittleEndian";
	AddDefinableProperty(PropertyContainer::enumTypeId(),"FileType",0,"enumNames",typeList_);

	charsWritten_ = 0;
	setValid(false);
}

/*! \brief Creates a ne AnalysisFileOutput and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> AnalysisFileOutput::Create()
{
	return QSharedPointer<Asset>(new AnalysisFileOutput());
}

void AnalysisFileOutput::enteredScope()
{	
	AnalysisOutput::enteredScope();
}

/*! \brief Writes the input text to the underlying file followed by a new line character.*/
void AnalysisFileOutput::writeLine(QString text)
{
	writeText(text + "\n");
}
/*! \brief Writes the input text to the underlying file.
 *	\note The data written to the file is only flushed to disk once every CHARS_BEFORE_FLUSH 
 *	characters or when the current run ends.
 */
void AnalysisFileOutput::writeText(QString text)
{
	if(!isValid())
	{
		openFile();
	}
	charsWritten_ += text.size();
	(*outputFileStream_).writeRawData(text.toLatin1().constData(),text.size());
	//	Even though Analysis data are already written to the AnalysisFileOutput object, pressing save
	//	only works if the file buffer has been flushed to disk. Otherwise, it just copies the still empty
	//	temporary file to the path selected by the user.
	//if(charsWritten_ > CHARS_BEFORE_FLUSH)
	//{
		file_->flush();
		charsWritten_ = 0;
	//}
}
/*! \brief Writes data out to the underlying file as binary values (ie. short, int, double)
 *  @param csvData a comma separated series of values (ie. "2.4,1,5.65,2.0")
 *  @param csvTypes a comma separated series of types to use for the csvData (ie. "float,int,double,float")
 *	Valid types are short,int,long,float,double.  If more values appear in the
 *	csvData than there are types in csvTypes, the last type will be used for
 *	all remaining values (ie. "float,int" in the previous example is equivalent to "float,int,int,int").
 */
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

	//	Even though Analysis data are already written to the AnalysisFileOutput object, pressing save
	//	only works if the file buffer has been flushed to disk. Otherwise, it just copies the still empty
	//	temporary file to the path selected by the user.
	//if(charsWritten_ > CHARS_BEFORE_FLUSH)
	//{
		file_->flush();
		charsWritten_ = 0;
	//}
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

/*! \brief Creates a new AnalysisFileOutputWidget or AnalysisBinaryOutputWidget depending on the type
 *	of the underlying file data, sets its underlying file and returns an AnalysisOutputWidget pointer 
 *	to it.
 *	\details It is the callers responsibility to delete the widget returned from this function.
 */
AnalysisOutputWidget* AnalysisFileOutput::createWidget()
{
	if(!isValid())
	{
		openFile();
	}
	//Now that the file is opened, add a widget showing it to this run's container widget
	//Choose a regular or binary widget according to the file type
	QString fileType = typeList_.value(propertyContainer_->getPropertyValue("FileType").toInt(),"");
	bool textMode = (fileType == "Text");
	if(textMode)
	{
		AnalysisFileOutputWidget* outputWidget(new AnalysisFileOutputWidget(1000));
		outputWidget->setFile(file_->fileName());
		return outputWidget;
	}
	AnalysisBinaryOutputWidget* outputWidget(new AnalysisBinaryOutputWidget());
	outputWidget->setFile(file_->fileName());
	return outputWidget;
}

/*! \brief Called to end the analysis.  Closes the underlying file, which has the effect of flushing
 *	all data from the file buffer into the file.
 */
void AnalysisFileOutput::finishUp()
{
	if(!file_.isNull() && file_->isOpen())
	{
		file_->close();
	}
	charsWritten_ = 0;
	setValid(false);
}

/*! \brief Opens a temporary file in the a subdirectory of the directory returned from getTempOutputDir()
 *	\details If the file is successfully opened, setValid(true) is called.  If it cannot be opened a
 *	message box to that effect pops up for the user and the function ends.
 *	\note The file must be opened in the correct mode, text or binary.  Binary files opened in text
 *	mode, for example, convert all 0x0A values to 0x0D0A.  This function takes care of that.
 */
void AnalysisFileOutput::openFile()
{
	//If the output sub directory for this file doesn't exist yet, make it.
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
	bool fileOpened = false;
	try{
		fileOpened = file_->open(textMode?(QIODevice::WriteOnly | QIODevice::Text):QIODevice::WriteOnly);
	} catch(...)
	{
		//Do nothing here.  Just catch the exception...
	}
	if(!fileOpened)
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
