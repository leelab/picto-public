#ifndef _AnalysisNumberVariable_H_
#define _AnalysisNumberVariable_H_

#include "../common.h"

#include "AnalysisVariable.h"

namespace Picto {

/*! \brief A NumberVariable for Analyses.
 *	\details Works exactly like the NumberVariable with exception that it is part of an Analysis.
 *	\sa NumberVariable
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisNumberVariable : public AnalysisVariable
#else
class AnalysisNumberVariable : public AnalysisVariable
#endif
{
	Q_OBJECT
	/*! \brief A script property used to read and write this AnalysisNumberVariable's value.*/
	Q_PROPERTY(double value READ getValue WRITE setValue)

public:
	AnalysisNumberVariable();
	virtual ~AnalysisNumberVariable(){};

	static QSharedPointer<Asset> Create();
	/*! \brief Returns this AnalysisNumberVariable's value.*/
	double getValue(){return propertyContainer_->getPropertyValue("Value").toDouble();};
	/*! \brief Sets this AnalysisNumberVariable's value.*/
	void setValue(double val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString getUITemplate(){return "NumberVariable";};
	virtual QString friendlyTypeName(){return "Number Variable";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
