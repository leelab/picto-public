#ifndef _AnalysisFileOutput_H_
#define _AnalysisFileOutput_H_

#include "../common.h"

#include "AnalysisOutput.h"
#include "AnalysisVariable.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
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

	virtual void reset();

	virtual QString friendlyTypeName(){return "File Output";};

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
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	virtual AnalysisOutputWidget* createWidget();
protected slots:
	void finishUp();

private:
	void openFile();
	void setValid(bool isValid){valid_ = isValid;};
	bool isValid(){return valid_;};
	unsigned int charsWritten_;
	QStringList typeList_; 
	QSharedPointer<QFile> file_;
	QSharedPointer<QDataStream> outputFileStream_;
	bool valid_;
};


}; //namespace Picto

#endif
