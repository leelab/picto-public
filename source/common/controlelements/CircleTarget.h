#ifndef _CIRCLETARGET_H_
#define _CIRCLETARGET_H_

#include "../common.h"
#include "controltarget.h"
#include <QRect>

namespace Picto {

/*!	\brief A CircleTarget is used by ChoiceController
 *	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API CircleTarget : public ControlTarget
#else
class CircleTarget : public ControlTarget
#endif
{
	Q_OBJECT
public:
	CircleTarget();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new CircleTarget());};
	virtual QString assetType(){return "CircleTarget";};
	void draw();
	virtual bool contains(int x, int y);
//These public slots exist for binding visual element properties to scripts
public slots:
	int getRadius();
	void setRadius(int radius);
protected:
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	bool active_;
};


}; //namespace Picto

#endif
