#ifndef _CONTROLTARGET_H_
#define _CONTROLTARGET_H_
#include <QRect>

#include "../common.h"
#include "../stimuli/VisualElement.h"

namespace Picto {

/*!	\brief A ControlTarget is used by ChoiceController
 *	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API ControlTarget : public VisualElement
#else
class ControlTarget : public VisualElement
#endif
{
	Q_OBJECT
	Q_PROPERTY(bool outside READ getOutside WRITE setOutside)
public:
	ControlTarget(QPoint position=QPoint(), QColor color=Qt::green);
	virtual QString getUITemplate(){return "ControlTarget";};
	virtual QString assetType(){return "ControlTarget";};
	void draw() = 0;
	void setActive(bool active);
	bool isActive(){return active_;};
	void setOutside(bool outside){propertyContainer_->setPropertyValue("Outside",outside); };
	bool getOutside(){return propertyContainer_->getPropertyValue("Outside").toBool();};
	virtual bool contains(int x, int y) = 0;
//These public slots exist for binding visual element properties to scripts
public slots:
	virtual bool getVisible(){return active_ && VisualElement::getVisible();};
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	bool active_;
};


}; //namespace Picto

#endif
