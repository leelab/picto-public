#ifndef _UIElement_H_
#define _UIElement_H_
#include <QUuid>

#include "../common.h"
#include "../storage/DataStore.h"
#include "../parameter/AssociateElement.h"

namespace Picto {

/*!	\brief An AssociateElement used to store UI information for its linked Asset's representation in the Designer UI.
 *	\details This element stores the x,y position of its linked Asset within the Designer canvas.  It also stores 
 *	a "notes" string that is displayed when an Asset is selected and in a tooltip displayed when the mouse hovers
 *	over the linked asset.
 *
 *	By separating UIElement's from the Design, we can remove them when the design is run in an experiment.
 *	Theoretically, although this is not yet implemented, we could also store multiple UI represntations of the same
 *	Design, although that may not be all that useful.  In general, separating UI information from Design information
 *	is good programming practice and ends up solving a whole host of problems that are not usually obvious at first 
 *	glance.
 *
 *	On the design side, the UIEnabled class knows how to interface with UIElement objects in order to retrieve UI information.
 *	\sa UIEnabled
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API UIElement : public DataStore, public AssociateElement
#else
class UIElement : public DataStore, public AssociateElement
#endif
{
	Q_OBJECT
public:
	UIElement();
	virtual ~UIElement(){};

	static QSharedPointer<Asset> Create();
	void setPos(QPoint pos);
	QPoint getPos();
	void setNotes(QString notes);
	QString getNotes();
	virtual QString friendlyTypeName(){return "UI Element";};

	ASSOCIATE_ELEMENT_IMPLEMENTATION

protected:
	virtual QString defaultTagName(){return "UIElement";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
};


}; //namespace Picto

#endif
