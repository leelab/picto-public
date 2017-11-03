#ifndef _AnalysisMatlabObj_H_
#define _AnalysisMatlabObj_H_

#include "../common.h"

#include "AnalysisVariable.h"

namespace Picto {

/*! \brief This is a wrapper that can be used to process data in Matlab.
*          We use the Matlab Coder tool for c++ generation that supports a subset 
*          of the MATLAB Language focused on algorithms performing numerical computations. 
*          We add here new matlab function wrappers, on demand.
*
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
	QVariantList binocdf(QVariantList in1, QVariantList in2, QVariantList in3);
	QVariantList binopdf(QVariantList in1, QVariantList in2, QVariantList in3);
	QVariantList rand(double in);
	QVariantList normrnd(QVariantList mu, QVariantList sigma);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};


}; //namespace Picto

#endif
