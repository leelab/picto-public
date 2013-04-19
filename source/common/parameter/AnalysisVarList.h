#ifndef _AnalysisVarList_H_
#define _AnalysisVarList_H_

#include "../common.h"

#include "AnalysisVariable.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API AnalysisVarList : public AnalysisVariable
#else
class AnalysisVarList : public AnalysisVariable
#endif
{
	Q_OBJECT
	Q_PROPERTY(QVariant value READ getValue WRITE setValue)

public:
	AnalysisVarList();
	virtual ~AnalysisVarList(){};

	static QSharedPointer<Asset> Create();

	QVariant getValue(){return QVariant();};
	void setValue(QVariant val){};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

};


}; //namespace Picto

#endif
