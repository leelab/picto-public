#ifndef _CONTROLTARGET_H_
#define _CONTROLTARGET_H_

#include "../common.h"
#include "../storage/DataStore.h"
#include "../statemachine/result.h"
#include <QRect>

namespace Picto {

/*!	\brief A ControlTarget is used by ChoiceController
 *	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API ControlTarget : public Result
#else
class ControlTarget : public Result
#endif
{
public:
	ControlTarget();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new ControlTarget());};
	QRect getBounds();

	virtual QString assetType(){return "ControlTarget";};

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
