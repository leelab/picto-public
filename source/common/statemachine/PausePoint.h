#ifndef _PAUSEPOINT_H_
#define _PAUSEPOINT_H_

#include "../common.h"
#include "OutputElementContainer.h"
#include "../engine/PictoEngine.h"
#include "scene.h"

namespace Picto {

/*!	\brief A PausePoint is the place where execution is suspended when the operator presses pause during an Experiment.
 *	\details Picto Tasks are tightly timed such that it really does not make sense to allow the operator to pause execution at any
 *	time.  If an Experiment were paused while a Fixation Point appeared on screen for example, we would effectively be giving the
 *	subject extra time to fixate on that point.  Ideally, we should be able to pause execution without affecting the integrity of 
 *	Session data afterward, and that is the purpose of this element.  PausePoints are the place in control flow where execution actually pauses after the 
 *	pause button is pressed.  In a Task with a trial structure, for example, the PausePoint should be placed just before or after the
 *	State handling the inter-trial interval so that any time pause is pressed the ITI will just be slightly longer and actual 
 *	experimental data will not be affected.
 *
 *	Sometimes it is useful to be able to set up an automatic pause point as well.  For example, when we need data in blocks of a specific
 *	number N of trials, we can set up a PausePoint to automatically Pause execution as soon as the Trial counter reaches N.  This way
 *	the operator doesn't have to keep an eagle eye on the trial number, but can focus on other tasks with the confidence that execution will
 *	stop when the time is right.  For this purpose, PausePoint objects contain a "ForcePause" Property which, when set 'true' causes 
 *	execution to Pause when the PausePoint is reached regardless of whether the pause button was pressed.
 *
 *	Regardless of whether pausing occured due to the pause button being pressed or "ForcePause" being true, pressing the play button causes
 *	playback to resume.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API PausePoint : public OutputElementContainer
#else
class PausePoint : public OutputElementContainer
#endif
{
	Q_OBJECT
public:
	PausePoint();
	virtual ~PausePoint(){};
	static QSharedPointer<Asset> Create();

	virtual void enableRunMode(bool enable);
	QString run(QSharedPointer<Engine::PictoEngine> engine);
	QString slaveRun(QSharedPointer<Engine::PictoEngine> engine);
	QString slaveRenderFrame(QSharedPointer<Engine::PictoEngine> engine);

	virtual QString getUITemplate(){return "PausePoint";};
	virtual QString friendlyTypeName(){return "Pause Point";};

	virtual void upgradeVersion(QString deserializedVersion);

	virtual void setDesignConfig(QSharedPointer<DesignConfig> designConfig);

	/*! \brief Returns the background color that will be displayed when execution is paused (not currently used).*/
	QColor getColor(){return propertyContainer_->getPropertyValue("BackgroundColor").value<QColor>();};
	/*! \brief Sets the background color that will be displayed when execution is paused (not currently used).*/
	void setColor(QColor color){propertyContainer_->setPropertyValue("BackgroundColor",color);};
	/*! \brief Returns the red component of the background color that will be displayed when execution is paused (not currently used).*/
	int getRed() { return getColor().red(); };
	/*! \brief Returns the green component of the background color that will be displayed when execution is paused (not currently used).*/
	int getGreen() { return getColor().green(); };
	/*! \brief Returns the blue component of the background color that will be displayed when execution is paused (not currently used).*/
	int getBlue() { return getColor().blue(); };
	/*! \brief Returns the alpha component of the background color that will be displayed when execution is paused (not currently used).*/
	int getAlpha() { return getColor().alpha(); };
	/*! \brief Sets the red component of the background color that will be displayed when execution is paused (not currently used).*/
	void setRed(int r){QColor val = getColor(); val.setRed(r);setColor(val);};
	/*! \brief Sets the green component of the background color that will be displayed when execution is paused (not currently used).*/
	void setGreen(int g){QColor val = getColor(); val.setGreen(g);setColor(val);};
	/*! \brief Sets the blue component of the background color that will be displayed when execution is paused (not currently used).*/
	void setBlue(int b){QColor val = getColor(); val.setBlue(b);setColor(val);};
	/*! \brief Sets the alpha component of the background color that will be displayed when execution is paused (not currently used).*/
	void setAlpha(int a){QColor val = getColor(); val.setAlpha(a);setColor(val);};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	void rebuildScene();
	QSharedPointer<Scene> scene_;
	QSharedPointer<SignalChannel> sigChannel_;
	bool hasCursor_;
private slots:
	void activeAnalysisIdsChanged();
};


}; //namespace Picto

#endif
