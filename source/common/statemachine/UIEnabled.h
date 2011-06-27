#ifndef _UIENABLED_H_
#define _UIENABLED_H_

#include "../common.h"
#include "../storage/DataStore.h"

namespace Picto {

/*!	\brief A UIEnabled 
 *	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API UIEnabled : public DataStore
#else
class UIEnabled : public DataStore
#endif
{
	Q_OBJECT
public:
	UIEnabled();

	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new UIEnabled());};
	virtual QString getName(){return propertyContainer_->getPropertyValue("Name").toString();};
	void setName(QString newName){propertyContainer_->setPropertyValue("Name",newName);};
	void setPos(QPoint pos);
	QPoint getPos();
	virtual QString assetType(){return "UIEnabled";};
signals:
	void nameEdited();

protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
