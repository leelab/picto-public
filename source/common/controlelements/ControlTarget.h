#ifndef _CONTROLTARGET_H_
#define _CONTROLTARGET_H_
#include <QRect>

#include "../common.h"
#include "../stimuli/VisualElement.h"

namespace Picto {

/*!	\brief A ControlTarget is a VisualElement that defines a fixation region and is invisible to the subject by default.
 *	\details ControlTarget elements are meant to be used with TargetController and ChoiceController elements to define
 *	fixation regions.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API ControlTarget : public VisualElement
#else
class ControlTarget : public VisualElement
#endif
{
	Q_OBJECT
	/*! \brief Sets/Gets whether the target location will be the region outside of this element's geometry.*/
	Q_PROPERTY(bool outside READ getOutside WRITE setOutside)
public:
	ControlTarget(QPoint position=QPoint(), QColor color=Qt::green);
	virtual QString getUITemplate(){return "ControlTarget";};
	virtual QString friendlyTypeName(){return "Control Target";};
	virtual QString getUIGroup(){return "Logic Elements";};
	void draw() = 0;
	void setActive(bool active);
	/*! \brief Returns whether this ControlTarget is currently being actively monitored by a ControlElement.*/
	bool isActive(){return active_;};
	/*! \brief Sets whether the target location will be the region outside of this element's geometry.*/
	void setOutside(bool outside){propertyContainer_->setPropertyValue("Outside",outside); };
	/*! \brief Gets whether the target location will be the region outside of this element's geometry.*/
	bool getOutside(){return propertyContainer_->getPropertyValue("Outside").toBool();};
	/*! \brief Returns whether the input x,y position is inside this ControlTarget element's target location.*/
	virtual bool contains(int x, int y) = 0;
//These public slots exist for binding visual element properties to scripts
public slots:
	/*! \brief Returns whether this ControlTarget is currently visible.
	 *	\details In the case of a ControlTarget, even if is is set as visible, it is only actually
	 *	visible if it is being actively monitored by a ControlElement.  This funciton returns true
	 *	only if the ControlTarget is both set as visible and currently being monitored by a 
	 *	ControlElement.
	 */
	virtual bool getVisible(){return active_ && VisualElement::getVisible();};
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	bool active_;
};


}; //namespace Picto

#endif
