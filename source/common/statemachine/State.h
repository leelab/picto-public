#ifndef _STATE_H_
#define _STATE_H_

#include "../common.h"
#include "StateMachineElement.h"
#include "Canvas.h"
#include "Layer.h"
#include "../controlelements/ControlElement.h"
#include "../controlelements/ControlLink.h"
#include "scene.h"
#include "../engine/PictoEngine.h"



#include <QScriptEngine>

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
class PICTOLIB_API State : public StateMachineElement
#else
class State : public StateMachineElement
#endif
{
public:
	State();
	static QSharedPointer<Asset> Create(){return QSharedPointer<Asset>(new State());};

	QString run(QSharedPointer<Engine::PictoEngine> engine);
	QString runAsSlave(QSharedPointer<Engine::PictoEngine> engine);

	//bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter);
	//bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader);


	void setRevision(int rev) { revision_ = rev; };
	int getRevision() { return revision_; };
	void setEngineNeeded(int eng) { engineNeeded_ = eng; };
	int getEngineNeeded() { return engineNeeded_; };

	void addControlLink(QSharedPointer<ControlLink> link);
	void addControlElement(QSharedPointer<ControlElement> controlElement);
	void removeControlElement(QString controlElementName);

	void setScene(QSharedPointer<Scene> scene) { scene_ = scene; };

	bool initScripting(QScriptEngine &qsEngine);
	virtual QString assetType(){return "State";};

protected:
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void sendBehavioralData(QSharedPointer<Engine::PictoEngine> engine);
	void runScript(QString scriptName);
	bool checkForEngineStop(QSharedPointer<Engine::PictoEngine> engine);
	//void updateServer(QSharedPointer<Engine::PictoEngine> engine, bool paused=false);
	int getMasterFramenumber(QSharedPointer<Engine::PictoEngine> engine);
	void addCursor();
	void reset();

	QSharedPointer<Scene> scene_;
	QMap<QString, QSharedPointer<ControlElement> > controlElements_;
	int revision_;
	int engineNeeded_;

	int frameCounter_;
	double lastFrameCheckTime_;
	bool hasCursor_;

	QSharedPointer<SignalChannel> sigChannel_;

	QSharedPointer<QScriptEngine> qsEngine_;
	QMultiMap<QString, QSharedPointer<ControlLink> > links_; //<source, link>
};


}; //namespace Picto

#endif
