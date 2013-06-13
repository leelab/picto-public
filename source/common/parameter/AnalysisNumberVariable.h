#ifndef _AnalysisNumberVariable_H_
#define _AnalysisNumberVariable_H_

#include "../common.h"

#include "AnalysisVariable.h"

namespace Picto {

/*! \brief A Variant Variable.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisNumberVariable : public AnalysisVariable
#else
class AnalysisNumberVariable : public AnalysisVariable
#endif
{
	Q_OBJECT
	Q_PROPERTY(double value READ getValue WRITE setValue)

public:
	AnalysisNumberVariable();
	virtual ~AnalysisNumberVariable(){};

	static QSharedPointer<Asset> Create();

	double getValue(){return propertyContainer_->getPropertyValue("Value").toDouble();};
	void setValue(double val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString friendlyTypeName(){return "Number Variable";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
