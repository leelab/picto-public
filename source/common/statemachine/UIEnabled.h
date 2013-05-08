#ifndef _UIENABLED_H_
#define _UIENABLED_H_
#include <QUuid>

#include "../common.h"
#include "../storage/DataStore.h"
#include "../statemachine/transition.h"

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
	virtual ~UIEnabled(){};

	static QSharedPointer<Asset> Create();
	virtual QString getName(){return propertyContainer_->getPropertyValue("Name").toString();};
	virtual void setName(QString newName){propertyContainer_->setPropertyValue("Name",newName);};
	void setPos(QPoint pos);
	QPoint getPos();
	virtual bool hasEditableDescendants(){return false;};
	virtual QString getUITemplate(){return "UIEnabled";};
	virtual QString friendlyTypeName(){return "UI Enabled";};
	virtual QString getUIGroup(){return "";};

	virtual void upgradeVersion(QString deserializedVersion);
signals:
	void nameEdited();

protected:
	virtual void preSerialize();
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool executeSearchAlgorithm(SearchRequest searchRequest);
private:
	QUuid uiAssociateId_;
	QSharedPointer<Asset> getUIElement();
};


}; //namespace Picto

#endif
