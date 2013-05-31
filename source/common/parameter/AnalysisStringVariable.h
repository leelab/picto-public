#ifndef _AnalysisStringVariable_H_
#define _AnalysisStringVariable_H_

#include "../common.h"

#include "AnalysisVariable.h"

namespace Picto {

/*! \brief A Variant Variable.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisStringVariable : public AnalysisVariable
#else
class AnalysisStringVariable : public Variable
#endif
{
	Q_OBJECT
	Q_PROPERTY(QString value READ getValue WRITE setValue)

public:
	AnalysisStringVariable();
	virtual ~AnalysisStringVariable(){};

	static QSharedPointer<Asset> Create();

	QString getValue(){return propertyContainer_->getPropertyValue("Value").toString();};
	void setValue(QString val){propertyContainer_->setPropertyValue("Value",val);};

	virtual QString friendlyTypeName(){return "String Variable";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
