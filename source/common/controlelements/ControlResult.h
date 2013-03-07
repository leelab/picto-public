#ifndef _CONTROLRESULT_H_
#define _CONTROLRESULT_H_

#include "../common.h"
#include "../storage/DataStore.h"
#include "../statemachine/requiredresult.h"
#include "controltarget.h"
#include <QRect>

namespace Picto {

/*!	\brief A ControlResult is used by ChoiceController
 *	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API ControlResult : public RequiredResult
#else
class ControlResult : public RequiredResult
#endif
{
	Q_OBJECT
public:
	ControlResult();
	virtual ~ControlResult(){};
	static QSharedPointer<Asset> Create();

	virtual QString assetType(){return "ControlResult";};
	bool contains(int x, int y);
	void setActive(bool active);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual void scriptableContainerWasReinitialized();
private:
	QWeakPointer<ControlTarget> controlTarget_;
};


}; //namespace Picto

#endif
