#ifndef _UIInfo_H_
#define _UIInfo_H_
#include <QUuid>

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
	Q_OBJECT
public:
	UIInfo();
	virtual ~UIInfo(){};

	static QSharedPointer<Asset> Create();

	void setPos(QPoint pos);
	QPoint getPos();
	void setOpenDescendant(int assetId);
	int getOpenDescendant();
	virtual QString assetType(){return "UIInfo";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
