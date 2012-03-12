#ifndef _ANALYSIS_OUTPUT_H_
#define _ANALYSIS_OUTPUT_H_

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

	//Should reset the AnalysisTool Object to its initial state.
	virtual void reset() = 0;
	//Should be overloaded to return a pointer to an output widget that
	//presents the information written into this AnalysisTool object.
	virtual QPointer<QWidget> getOutputWidget() = 0;

	//Inherited
	virtual QString getUITemplate(){return "AnalysisTool";};
	virtual QString assetType(){return "AnalysisTool";};

protected:

	//Inherited
	virtual QString defaultTagName(){return "AnalysisTool";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	void setValid(bool isValid){valid_ = isValid;};
	bool isValid(){return valid_;};
private:
	bool valid_;

};
}; //namespace Picto
#endif