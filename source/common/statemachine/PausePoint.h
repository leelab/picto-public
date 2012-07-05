#ifndef _PAUSEPOINT_H_
#define _PAUSEPOINT_H_

#include "../common.h"
#include "StateMachineElement.h"
#include "../engine/PictoEngine.h"
#include "scene.h"

namespace Picto {

/*!	\brief A PausePoint is the place where execution is suspended when pause is pressed
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API PausePoint : public StateMachineElement
#else
class PausePoint : public StateMachineElement
#endif
{
	Q_OBJECT
public:
	PausePoint();
	virtual ~PausePoint(){};
	static QSharedPointer<Asset> Create();

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	QString runAsSlave(QSharedPointer<Engine::PictoEngine> engine);
	virtual QString getUITemplate(){return "PausePoint";};
	virtual QString assetType(){return "PausePoint";};

	QColor getColor(){return propertyContainer_->getPropertyValue("BackgroundColor").value<QColor>();};
	void setColor(QColor color){propertyContainer_->setPropertyValue("BackgroundColor",color);};
	int getRed() { return getColor().red(); };
	int getGreen() { return getColor().green(); };
	int getBlue() { return getColor().blue(); };
	int getAlpha() { return getColor().alpha(); };
	void setRed(int r){QColor val = getColor(); val.setRed(r);setColor(val);};
	void setGreen(int g){QColor val = getColor(); val.setGreen(g);setColor(val);};
	void setBlue(int b){QColor val = getColor(); val.setBlue(b);setColor(val);};
	void setAlpha(int a){QColor val = getColor(); val.setAlpha(a);setColor(val);};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();
	virtual QMap<QString,QPair<QString,QString>>  getScripts();
	virtual void scriptableContainerWasReinitialized();

private:
	QSharedPointer<Scene> scene_;
	QSharedPointer<SignalChannel> sigChannel_;
	bool hasCursor_;
};


}; //namespace Picto

#endif
