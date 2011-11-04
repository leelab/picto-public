#ifndef _OPERATORCLICKPARAMETER_H_
#define _OPERATORCLICKPARAMETER_H_

#include "../common.h"

#include "parameter.h"

namespace Picto {

/*! \brief A boolean parameter.
 *
 *	This parameter can only hold the values true or false, although we can relable those
 *	values to anything we want (e.g. "On" and "Off").  The equality operators work as 
 *	expected for these parameters, but the greater than and less than operators will
 *	always return false, since theydoesn't make sense with boolean values.
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API OperatorClickParameter : public Parameter
#else
class OperatorClickParameter : public Parameter
#endif
{
	Q_OBJECT
	Q_PROPERTY(int x READ getX WRITE setX);
	Q_PROPERTY(int y READ getY WRITE setY);
	Q_PROPERTY(bool newClick READ getNewClick WRITE setNewClick);

public:

	static QSharedPointer<Asset> Create();

	int getX(){propertyContainer_->setPropertyValue("NewClick",false);return propertyContainer_->getPropertyValue("Pos").toPoint().x();};
	void setX(int){};
	int getY(){propertyContainer_->setPropertyValue("NewClick",false);return propertyContainer_->getPropertyValue("Pos").toPoint().y();};
	void setY(int){};
	bool getNewClick(){return propertyContainer_->getPropertyValue("NewClick").toBool();};
	void setNewClick(bool){};
	void setLastClick(QPoint point);
	static void addClick(QPoint point);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	OperatorClickParameter();
	static QList<QWeakPointer<OperatorClickParameter>> params_;

};


}; //namespace Picto

#endif
