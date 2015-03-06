#ifndef _AnalysisStringVariable_H_
#define _AnalysisStringVariable_H_

#include "../common.h"

#include "AnalysisVariable.h"

namespace Picto {

/*! \brief A StringVariable for Analyses.
 *	\details Works exactly like the StringVariable with the exception that it is part of an Analysis.
 *	\sa StringVariable
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisStringVariable : public AnalysisVariable
#else
class AnalysisStringVariable : public Variable
#endif
{
	Q_OBJECT
	/*! \brief A script property used to read and write this AnalysisStringVariable's value.*/
	Q_PROPERTY(QString value READ getValue WRITE setValue)

public:
	AnalysisStringVariable();
	virtual ~AnalysisStringVariable(){};

	static QSharedPointer<Asset> Create();
	/*! \brief Returns this AnalysisStringVariable's value.*/
	QString getValue(){return propertyContainer_->getPropertyValue("Value").toString();};
	/*! \brief Sets this AnalysisStringVariable's value.*/
	void setValue(QString val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString getUITemplate(){return "StringVariable";};
	virtual QString friendlyTypeName(){return "String Variable";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
