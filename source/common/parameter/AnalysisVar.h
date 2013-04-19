#ifndef _ANALYSISVAR_H_
#define _ANALYSISVAR_H_

#include "../common.h"

#include "AnalysisVariable.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisVar : public AnalysisVariable
#else
class AnalysisVar : public AnalysisVariable
#endif
{
	Q_OBJECT
	Q_PROPERTY(QVariant value READ getValue WRITE setValue)

public:
	AnalysisVar();
	virtual ~AnalysisVar(){};

	static QSharedPointer<Asset> Create();

	virtual void reset();
	QVariant getValue(){return val_;};
	void setValue(QVariant val){val_ = val;};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QVariant val_;
};


}; //namespace Picto

#endif
