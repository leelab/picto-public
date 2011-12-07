#ifndef _STATE_H_
#define _STATE_H_

#include "../common.h"
#include "MachineContainer.h"
#include "../controlelements/ControlElement.h"
#include "../controlelements/ControlLink.h"
#include "scene.h"
#include "../engine/PictoEngine.h"

namespace Picto {

class BehavioralDataUnitPackage;
class CommandChannel;
class SignalChannel;

/*!	\brief The stat machine element that handles stimulus rendering
 *
 *	A state contatins a single scene and multiple control elements.  This is where
 *	the rendering loop lives.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API State : public MachineContainer
#else
class State : public MachineContainer
#endif
{
	Q_OBJECT
	//Q_PROPERTY(int red READ getRed WRITE setRed)
	//Q_PROPERTY(int green READ getGreen WRITE setGreen)
	//Q_PROPERTY(int blue READ getBlue WRITE setBlue)
	//Q_PROPERTY(int alpha READ getAlpha WRITE setAlpha)
public:
	State();
	virtual ~State(){};
	static QSharedPointer<Asset> Create();

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	QString runAsSlave(QSharedPointer<Engine::PictoEngine> engine);

	void setRevision(int rev) { revision_ = rev; };
	int getRevision() { return revision_; };
	void setEngineNeeded(int eng) { engineNeeded_ = eng; };
	int getEngineNeeded() { return engineNeeded_; };

	void addControlLink(QSharedPointer<ControlLink> link);
	void addControlElement(QSharedPointer<ControlElement> controlElement);

	void setScene(QSharedPointer<Scene> scene) { scene_ = scene; };

	virtual QString getUITemplate(){return "State";};
	virtual QString assetType(){return "State";};

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

//public slots:
//	void setBackgroundColor(int r, int g, int b, int a=255){setColor(QColor(r,g,b,a));};

protected:
	virtual void elementAdded(QSharedPointer<ResultContainer> element){};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();
	//This returns a map of QMap<script name,script code>
	virtual QMap<QString,QString> getScripts();
	virtual void scriptableContainerWasReinitialized();


private:
	void sendBehavioralData(QSharedPointer<Engine::PictoEngine> engine);
	//void runScript(QString scriptName);
	bool checkForEngineStop(QSharedPointer<Engine::PictoEngine> engine);
	//void updateServer(QSharedPointer<Engine::PictoEngine> engine, bool paused=false);
	void addCursor();

	QSharedPointer<Scene> scene_;
	//QMap<QString, QSharedPointer<ControlElement> > controlElements_;
	int revision_;
	int engineNeeded_;

	QString lastFrameCheckTime_;
	bool hasCursor_;

	QSharedPointer<SignalChannel> sigChannel_;

	QMultiMap<QString, QSharedPointer<ControlLink> > links_; //<source, link>
};


}; //namespace Picto

#endif
