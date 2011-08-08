#ifndef _RECTTARGET_H_
#define _RECTTARGET_H_

#include "../common.h"
#include "controltarget.h"
#include <QRect>

namespace Picto {

/*!	\brief A RectTarget is used by ChoiceController
 *	
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API RectTarget : public ControlTarget
#else
class RectTarget : public ControlTarget
#endif
{
	Q_OBJECT
public:
	RectTarget();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new RectTarget());};
	QRect getBounds();
	virtual QString assetType(){return "RectTarget";};
	void draw();
	virtual bool contains(int x, int y);
//These public slots exist for binding visual element properties to scripts
public slots:
	int getWidth() { return getBounds().width(); };
	void setWidth(int width);
	int getHeight() { return getBounds().height(); };
	void setHeight(int height);
protected:
	void setBounds(QRect bounds);
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	bool active_;
};


}; //namespace Picto

#endif
