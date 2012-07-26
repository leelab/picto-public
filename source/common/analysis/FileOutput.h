#ifndef _FILE_OUTPUT_H_
#define _FILE_OUTPUT_H_

#include <QFile>
#include <QDataStream>
#include <QSharedPointer>
#include <QTextEdit>
#include "AnalysisOutput.h"
#include "FileOutputWidget.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API FileOutput : public AnalysisOutput
#else
class FileOutput : public AnalysisOutput
#endif
{
	Q_OBJECT
public:
	FileOutput();
	virtual ~FileOutput();
	static QSharedPointer<Asset> Create();
	
	//Returns a pointer to an output widget that
	//presents the information written into this AnalysisTool object.
	QPointer<AnalysisOutputWidget> getOutputWidget();
	//bool saveOutputData(QString directory, QString filename);
	void finishUp();

	//Inherited
	virtual QString getUITemplate(){return "FileOutput";};
	virtual QString assetType(){return "FileOutput";};

public slots:
	void writeLine(QString text);
	void writeText(QString text);
	/*! \brief Writes data out to file as binary values (ie. short, int, double)
	 *  @param csvData a comma separated series of values.
	 *  @param csvTypes a comma separated series of types to use for the csvData.
	 *	Valid types are short,int,long,float,double.  If more values appear in the
	 *	csvData than there are types in csvTypes, the last type will be used for
	 *	all remaining values.
	 */
	void writeBinary(QString csvData,QString csvTypes);

protected:

	//Inherited
	//Should reset the FileOutput Object to its initial state.
	void reset();
	virtual QString defaultTagName(){return "FileOutput";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	unsigned int charsWritten_;
	QSharedPointer<QFile> file_;
	QSharedPointer<QDataStream> outputFileStream_;


};
}; //namespace Picto
#endif