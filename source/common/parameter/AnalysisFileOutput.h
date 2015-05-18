#ifndef _AnalysisFileOutput_H_
#define _AnalysisFileOutput_H_

#include "../common.h"

#include "AnalysisOutput.h"
#include "AnalysisVariable.h"

namespace Picto {

/*! \brief A simple AnalysisOutput object used for writing text or binary data to files.
 *	\details This object will take any text or binary data written through its writeLine(), 
 *	writeText() or writeBinary() functions and write it to a file.  The filename is the same
 *	as the current run name, and the file suffix is settable as a Property.  The file can be 
 *	set as a text file or a binary file, with both big-endian and little-endian binary options.
 *	
 *	For in Picto viewing of output data, this object returns an AnalysisFileWidget or 
 *	AnalysisBinaryWidget from createWidget depending on the file type.
 *	\sa AnalysisFileOutputWidget, AnalysisOutputWidgetContainer
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisFileOutput : public AnalysisOutput
#else
class AnalysisFileOutput : public AnalysisOutput
#endif
{
	Q_OBJECT

public:
	AnalysisFileOutput();
	virtual ~AnalysisFileOutput(){};

	static QSharedPointer<Asset> Create();

	//Returns a pointer to an output widget that
	//presents the information written into this AnalysisTool object.
	//QPointer<AnalysisOutputWidget> getOutputWidget();

	virtual void enteredScope();

	virtual QString friendlyTypeName(){return "File Output";};

public slots:
	void writeLine(QString text);
	void writeText(QString text);
	void writeBinary(QString csvData,QString csvTypes);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	virtual AnalysisOutputWidget* createWidget();

	virtual void finishUp();

private:
	void openFile();
	/*! \brief Sets whether the file associated with this object is open and ready for data.*/
	void setValid(bool isValid){valid_ = isValid;};
	/*! \brief Returns whether the file associated with this object is open and ready for data.*/
	bool isValid(){return valid_;};
	unsigned int charsWritten_;
	QStringList typeList_; 
	QSharedPointer<QFile> file_;
	QSharedPointer<QDataStream> outputFileStream_;
	bool valid_;
};


}; //namespace Picto

#endif
