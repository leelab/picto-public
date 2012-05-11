#ifndef _ANALYSIS_TOOL_H_
#define _ANALYSIS_TOOL_H_

#include <QScriptValue>
#include <QWidget>
#include <QPointer>
#include "../statemachine/Scriptable.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API AnalysisTool : public Scriptable
#else
class AnalysisTool : public Scriptable
#endif
{
	Q_OBJECT
public:
	AnalysisTool();
	virtual ~AnalysisTool();

	void initialize(QString runName,QUuid runUuid){runName_ = runName;runUuid_=runUuid;reset();};

	//Inherited
	virtual QString getUITemplate(){return "AnalysisTool";};
	virtual QString assetType(){return "AnalysisTool";};

protected:

	//Should reset the AnalysisTool Object to its initial state.
	virtual void reset() = 0;

	//Inherited
	virtual QString defaultTagName(){return "AnalysisTool";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	void setValid(bool isValid){valid_ = isValid;};
	bool isValid(){return valid_;};
	QString runName_;
	QUuid runUuid_;
private:
	bool valid_;

};
}; //namespace Picto
#endif