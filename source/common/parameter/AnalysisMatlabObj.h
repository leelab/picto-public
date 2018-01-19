#ifndef _AnalysisMatlabObj_H_
#define _AnalysisMatlabObj_H_

#include "../common.h"

#include "AnalysisVariable.h"

namespace Picto {

/*! \brief This is a base class for all classes that can be used to process data in Matlab
*	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
*	\date 2009-2017
*/
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisMatlabObj : public AnalysisVariable
#else
class AnalysisMatlabObj : public AnalysisVariable
#endif
{
	Q_OBJECT

public:
	AnalysisMatlabObj();
	virtual ~AnalysisMatlabObj();

	static QSharedPointer<Asset> Create();

	virtual void enteredScope();

	virtual QString friendlyTypeName(){ return "Matlab Function"; };
	virtual QString getUITemplate(){ return "MatlabObject"; }; //for the icon
	virtual QString getUIGroup(){ return "Logic Elements"; }; //for the section in the left column where the user selects objects to add to the xml design
		

public slots:	

	//MATLAB CODER
	QVariantList binocdfCoder(QVariantList in1, QVariantList in2, QVariantList in3);
	QVariantList binopdfCoder(QVariantList in1, QVariantList in2, QVariantList in3);
	QVariantList randCoder(double in);
	QVariantList normrndCoder(QVariantList mu, QVariantList sigma);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	void SDKInit();

	/*
	* MATLAB is a "multiple use" automation server, which means that multiple controller 
	* clients will connect to a single instance of the MATLAB ActiveX server application. 
	* For example, suppose you have two engOpen calls in one C program. When the first engOpen 
	* call is executed and attempts to establish a connection to MATLAB, Windows will look to see 
	* if a MATLAB ActiveX automation server is already running. If one is running, a connection 
	* will be established. If one is not running, it will launch MATLAB with the /Automation command line argument. 
	* When the second EngOpen call is executed, Windows will find the current MATLAB session and connect to it.
	* It is important to note that the two programs will share resources in the MATLAB application, including variables in the workspace.*/
	//Engine *matlabEngine_;

	bool doneInit_;
};


}; //namespace Picto

#endif
