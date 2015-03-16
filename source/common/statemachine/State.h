#ifndef _STATE_H_
#define _STATE_H_

#include <QMutex>
#include "../common.h"
#include "MachineContainer.h"
#include "../controlelements/ControlElement.h"
#include "scene.h"
#include "../engine/PictoEngine.h"

namespace Picto {

class CommandChannel;
class SignalChannel;

/*!	\brief The StateMachineElement that handles rendering of graphical, audio and signal elements and responding to test subject feedback.
 *	
 *	\details Apart from the PausePoint in which time passes only if the experiment is Paused, the State is the only StateMachineElement in which 
 *	experimental time actually passes.  Essentially, everything that happens in the StateMachine is happening in order to prepare for the next State execution.
 *	States render all in scope OutputElements every frame.  Beyond the usual EntryScript and ExitScript, States also contain a FrameScript that is
 *	run just before rendering each frame.  States contain ControlElement objects that monitor user input and time signals to see when certain events take place.  
 *	Before the FrameScript is called every frame, these ControlElements are checked to see if any of these events have taken place.  Execution continues in
 *	the State until one of those ControlElement events occurs at which point a corresponding result is triggered and the State is finished.  It should be 'nearly'
 *	possible, but extremely messy, to put all Task code into a single State.  Complicated ScriptControllers can always be used to define when the
 *	State ends, and complicated code can always be put into the FrameScript.  We say it is 'nearly' possible because it wouldn't be possible to do things like
 *	provide rewards, but it is important to note that it is 'nearly' true.  Of course, it would be incredibly messy to do this and no one ever should, 
 *	but the fact that it is 'nearly' possible should be an indication of the importance of this class.  With this in mind, be extremely careful before changing 
 *	anything about this class, particularly in the run() method.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API State : public MachineContainer
#else
class State : public MachineContainer
#endif
{
	Q_OBJECT
public:
	State();
	virtual ~State(){};
	static QSharedPointer<Asset> Create();

	virtual void enableRunMode(bool enable);
	QString run(QSharedPointer<Engine::PictoEngine> engine);
	QString slaveRun(QSharedPointer<Engine::PictoEngine> engine);
	QString slaveRenderFrame(QSharedPointer<Engine::PictoEngine> engine);

	void addControlElement(QSharedPointer<ControlElement> controlElement);

	virtual QString getUITemplate(){return "State";};
	virtual QString friendlyTypeName(){return "State";};

	/*! \brief Returns the background color that will be displayed (not currently used).*/
	QColor getColor(){return propertyContainer_->getPropertyValue("BackgroundColor").value<QColor>();};
	/*! \brief Sets the background color that will be displayed (not currently used).*/
	void setColor(QColor color){propertyContainer_->setPropertyValue("BackgroundColor",color);};
	/*! \brief Returns the red component of the background color that will be displayed (not currently used).*/
	int getRed() { return getColor().red(); };
	/*! \brief Returns the green component of the background color that will be displayed (not currently used).*/
	int getGreen() { return getColor().green(); };
	/*! \brief Returns the blue component of the background color that will be displayed (not currently used).*/
	int getBlue() { return getColor().blue(); };
	/*! \brief Returns the alpha component of the background color that will be displayed (not currently used).*/
	int getAlpha() { return getColor().alpha(); };
	/*! \brief Sets the red component of the background color that will be displayed (not currently used).*/
	void setRed(int r){QColor val = getColor(); val.setRed(r);setColor(val);};
	/*! \brief Sets the green component of the background color that will be displayed (not currently used).*/
	void setGreen(int g){QColor val = getColor(); val.setGreen(g);setColor(val);};
	/*! \brief Sets the blue component of the background color that will be displayed (not currently used).*/
	void setBlue(int b){QColor val = getColor(); val.setBlue(b);setColor(val);};
	/*! \brief Sets the alpha component of the background color that will be displayed (not currently used).*/
	void setAlpha(int a){QColor val = getColor(); val.setAlpha(a);setColor(val);};

	virtual void upgradeVersion(QString deserializedVersion);

	virtual void setDesignConfig(QSharedPointer<DesignConfig> designConfig);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual bool canHaveScripts(){return true;};
	virtual bool hasScripts();
	virtual QMap<QString,QString> getScripts();
	void runAnalysisFrameScripts();

	friend class SlaveExperimentDriver;

private:
	bool checkForEngineStop(QSharedPointer<Engine::PictoEngine> engine);
	void addCursor();

	void rebuildScene();

	QSharedPointer<Scene> scene_;

	bool hasCursor_;

	QSharedPointer<SignalChannel> sigChannel_;
	QMutex rebuildSceneMutex_;

private slots:
	void activeAnalysisIdsChanged();
};


}; //namespace Picto

#endif
