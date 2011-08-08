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
public:
	State();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new State());};

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

protected:
	virtual void elementAdded(QSharedPointer<ResultContainer> element){};
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool hasScripts();
	//This returns a map of QMap<script name,script code>
	virtual QMap<QString,QString> getScripts();
	virtual void scriptableContainerWasReinitialized();


private:
	void sendBehavioralData(QSharedPointer<Engine::PictoEngine> engine);
	//void runScript(QString scriptName);
	bool checkForEngineStop(QSharedPointer<Engine::PictoEngine> engine);
	//void updateServer(QSharedPointer<Engine::PictoEngine> engine, bool paused=false);
	int getMasterFramenumber(QSharedPointer<Engine::PictoEngine> engine);
	void addCursor();

	QSharedPointer<Scene> scene_;
	//QMap<QString, QSharedPointer<ControlElement> > controlElements_;
	int revision_;
	int engineNeeded_;

	int frameCounter_;
	double lastFrameCheckTime_;
	bool hasCursor_;

	QSharedPointer<SignalChannel> sigChannel_;

	QMultiMap<QString, QSharedPointer<ControlLink> > links_; //<source, link>
};


}; //namespace Picto

#endif
