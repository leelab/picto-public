#ifndef _UIENABLED_H_
#define _UIENABLED_H_
#include <QUuid>

#include "../common.h"
#include "../storage/DataStore.h"
#include "../statemachine/transition.h"

namespace Picto {

/*!	\brief A DataStore that can appear on the PictoDesigner canvas and that provides an option to show runtime editable Properties for in session editing.
 *	\details UIEnabled objects have "Name", "UIEnabled", and "UIOrder" Properties.  Where "Name" is used to identify the object, "UIEnabled" is a boolean
 *	indicating if the object's runtime Properties will be attached to widgets during session runs so that their init values can be updated.  "UIOrder"
 *	is not currently used but may someday be implemented to store the order of this objects runtime Properties in the widget window.
 *	UIEnabled objects also use their attached UIElement children to store a position in the Designer canvas and design notes.
 *	These are accessed with setNotes() and getNotes().
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
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

	/*! \brief Creates a new UIEnabled object and returns it as a shared pointer to an Asset.*/
	static QSharedPointer<Asset> Create();
	virtual QString getName(){return propertyContainer_->getPropertyValue("Name").toString();};
	/*! \brief Sets the name of this UIEnabled object.*/
	virtual void setName(QString newName){propertyContainer_->setPropertyValue("Name",newName);};
	void setPos(QPoint pos);
	QPoint getPos();
	void setNotes(QString notes);
	QString getNotes();
	/*! \brief Returns true if this object can or does have child objects that are editable.
	 *	\details This is useful, for example, so that the Designer knows if this object should be opened
	 *	up when the user double clicks on it or if there's nothing inside that can be changed so it
	 *	should not be opened.
	 */
	virtual bool hasEditableDescendants(){return false;};
	/*! \brief Returns the name of a user interface template that should be used to represent this element in a UI.*/
	virtual QString getUITemplate(){return "UIEnabled";};
	virtual QString friendlyTypeName(){return "UI Enabled";};
	/*! \brief Returns the name of a type of elements with which this element should be grouped in things like ToolBox widget.*/
	virtual QString getUIGroup(){return "";};

	virtual void upgradeVersion(QString deserializedVersion);
signals:
	/*! \brief Emitted when this object's name is changed.*/
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
