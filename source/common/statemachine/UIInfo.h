#ifndef _UIInfo_H_
#define _UIInfo_H_

#include "../common.h"
#include "../storage/DataStore.h"

namespace Picto {

/*!	\brief A UIInfo is the exit point from a StateMachineElement or ControlElement
 *	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API UIInfo : public DataStore
#else
class UIInfo : public DataStore
#endif
{
public:
	UIInfo();

	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new UIInfo());};

	void setPos(QPoint pos);
	QPoint getPos();
	virtual QString assetType(){return "UIInfo";};

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
