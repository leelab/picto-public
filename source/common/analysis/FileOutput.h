#ifndef _FILE_OUTPUT_H_
#define _FILE_OUTPUT_H_

#include <QFile>
#include <QTextStream>
#include <QSharedPointer>
#include <QTextEdit>
#include "AnalysisTool.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API FileOutput : public AnalysisTool
#else
class FileOutput : public AnalysisTool
#endif
{
	Q_OBJECT
public:
	FileOutput();
	virtual ~FileOutput();
	static QSharedPointer<Asset> Create();

	//Should reset the FileOutput Object to its initial state.
	void reset();
	
	//Returns a pointer to an output widget that
	//presents the information written into this AnalysisTool object.
	QPointer<QWidget> getOutputWidget();

	static QString getTempOutputDir();

	//Inherited
	virtual QString getUITemplate(){return "FileOutput";};
	virtual QString assetType(){return "FileOutput";};

public slots:
	void writeLine(QString text);
	void writeText(QString text);

protected:

	//Inherited
	virtual QString defaultTagName(){return "FileOutput";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	static void initTempOutputDir();
	static QString outputDir_;
	static int loadedObjects_;
	QSharedPointer<QFile> file_;
	QSharedPointer<QTextStream> outputFileStream_;
	QPointer<QWidget> outputWidget_;


};
}; //namespace Picto
#endif