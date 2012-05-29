#ifndef _ANALYSIS_OUTPUT_H_
#define _ANALYSIS_OUTPUT_H_

#include <QScriptValue>
#include <QWidget>
#include <QPointer>
#include <QUuid>
#include <QFile>
#include "AnalysisTool.h"
#include "AnalysisOutputWidget.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API AnalysisOutput : public AnalysisTool
#else
class AnalysisOutput : public AnalysisTool
#endif
{
	Q_OBJECT
public:
	AnalysisOutput();
	virtual ~AnalysisOutput();

	//Should be overloaded to return a pointer to an output widget that
	//presents the information written into this AnalysisOutput object.
	virtual QPointer<AnalysisOutputWidget> getOutputWidget(){return QPointer<AnalysisOutputWidget>();};
	//If saving is supported, should be overloaded to save data with input filename
	//to input directory.  Return true on success.  Input filename should not include
	//an extension.
	//virtual bool saveOutputData(QString directory, QString filename){return false;};
	//Called to end the analysis.  Should be overloaded to finish any operations that
	//need to be finished before analysis ends.
	virtual void finishUp(){};

	//Inherited
	virtual QString getUITemplate(){return "AnalysisOutput";};
	virtual QString assetType(){return "AnalysisOutput";};

	QString getTempOutputDir();

protected:

	//Inherited
	virtual QString defaultTagName(){return "AnalysisOutput";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	void setValid(bool isValid){valid_ = isValid;};
	bool isValid(){return valid_;};
	QString getOutputNamePrefix(){return runName_;};
private:
	static void initTempOutputDir();
	static void removeFilesThenDirectories(QDir container);
	static QString outputDir_;
	static int loadedObjects_;
	static QSharedPointer<QFile> lockFile_;	//File that is kept open while the application is active to prevent the directory from being deleted.
	bool valid_;

};
}; //namespace Picto
#endif