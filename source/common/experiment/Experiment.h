#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

#include "../common.h"

#include "../task/Task.h"
#include "../statemachine/scriptablecontainer.h"
#include "../statemachine/UIEnabled.h"
#include "../property/PropertyContainer.h"
#include "../parameter/AssociateRootHost.h"

#include <QList>
#include <QSharedPointer>

#include "../engine/PictoEngine.h"
#include "../engine/propertytable.h"


namespace Picto {

/*!	\brief The Experiment object contains the complete definition of an Experiment for running in the Director,
 *	TestViewer, RemoteViewer or ReplayViewer.
 *	
 *	This is the top level object in the Experiment tree.  Its parent is the PictoData object which holds Analysis objects
 *	and UIData objects as well.  Each PictoData object must contain one and only one Experiment, and this Experiment
 *	holds all of the Task objects that may be run during a session.  Experiment objects also hold a pointer to an
 *	Engine::PictoEngine object which "runs" the experiment, interfacing it with the various hardware being used in a
 *	particular session and with the network infrastructure that sends Experiment data to the PictoServer.
 *
 *	The Experiment is an AssociateRootHost so that UIData and Analysis objects can attach to it and be informed when
 *	something about the Experiment changes.  It also contains all of the Properties needed for position SignalChannel
 *	calibration for two reasons:
 *		- The calibration Properties need to be attached to an object that is always in scope and whose name never
 *	changes (it is always "Experiment") for the purpose of script access.  
 *		- The calibration Properties need to maintain their values regardless of which Task is running and where control
 *	flow is in a Task StateMachine, so their owner can never lose scope.
 *
 *	\note Unlike all other Property values in Picto, Experiment calibration Property initValues are linked to their
 *	runValues so that as soon as the operator changes a calibration value it changes the SignalChannel calibration and
 *	takes effect on the Experiment's position signal.  This is necessary since the Experiment never leaves scope.  If
 *	these Properties functioned like all other Properties, changes to their initValues would never actually take effect.
 *	\sa SignalChannel, Engine::PictoEngine, Task
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API Experiment : public ScriptableContainer, public AssociateRootHost
#else
class Experiment : public ScriptableContainer, public AssociateRootHost
#endif
{
	Q_OBJECT
	/*! \brief Sets/gets the offset that will be added to the input signal value before it is scaled (by xGain) for the
	 *	xPosition.
	 *	\details The equation is \code xPos = screenWidth/2 + xGain(xOffset+xSignalValue) \endcode 
	 *	\details By using this equation, we are assured that when the xPos is in the center of the screen, changing xGain
	 *	will not cause the current xPos to change.  This way, to calibrate the position we can use xOffset to move the
	 *	xPos to the center of the screen, then change xGain until xPos movements are accurate.
	 */
	Q_PROPERTY(double xOffset READ getXOffset WRITE setXOffset);
	/*! \brief Sets/gets the offset that will be added to the input signal value before it is scaled (by yGain) for the
	 *	yPosition.
	 *	\details The equation is \code yPos = screenHeight/2 + yGain(yOffset+ySignalValue) \endcode 
	 *	\details By using this equation, we are assured that when the yPos is in the center of the screen, changing yGain
	 *	will not cause the current yPos to change.  This way, to calibrate the position we can use yOffset to move the
	 *	yPos to the center of the screen, then change yGain until yPos movements are accurate.
	 */
	Q_PROPERTY(double yOffset READ getYOffset WRITE setYOffset);
	/*! \brief Sets/gets the gain by which the input signal value plus xOffset will be scaled for the xPosition.
	 *	\details The equation is \code xPos = screenWidth/2 + xGain(xOffset+xSignalValue) \endcode 
	 *	\details By using this equation, we are assured that when the xPos is in the center of the screen, changing xGain
	 *	will not cause the current xPos to change.  This way, to calibrate the position we can use xOffset to move the
	 *	xPos to the center of the screen, then change xGain until xPos movements are accurate.
	 */
	Q_PROPERTY(double xGain READ getXGain WRITE setXGain);
	/*! \brief Sets/gets the gain by which the input signal value plus yOffset will be scaled for the yPosition.
	 *	\details The equation is \code yPos = screenHeight/2 + yGain(yOffset+ySignalValue) \endcode 
	 *	\details By using this equation, we are assured that when the yPos is in the center of the screen, changing yGain
	 *	will not cause the current yPos to change.  This way, to calibrate the position we can use yOffset to move the
	 *	yPos to the center of the screen, then change yGain until yPos movements are accurate.
	 */
	Q_PROPERTY(double yGain READ getYGain WRITE setYGain);
	/*! \brief Sets/gets the shear factor that will be used to shear x as a function of y.
	 *	\details The x value will be sheared as a function of y after applying regular linear scale factors such that the
	 *	new value will be: \code xPos = scaledXPos + shearFactor*scaledYPos \endcode
	 *	This is useful for cases where there is some non linearity in the x,y position values.  Often, when position is
	 *	accurate in the y=0 line, but not toward the top/bottom display boundaries, this value can be used to clean up
	 *	the signal enough to make diagonal jumps relatively accurate
	 */
	Q_PROPERTY(double xySignalShear READ getXYSignalShear WRITE setXYSignalShear);
public:
	static QSharedPointer<Experiment> Create();
	virtual ~Experiment(){};
	/*! \brief Overrides UIEnabled::getName() to return the name of this Experiment as "Experiment" for the purpose of
	 *	scripting, regardless of the design file name or anything like that.
	 *	\details This way, any javascript code that wants to change position offsets and gains can do so by using code
	 *	like \code Experiment.xOffset = # \endcode.
	 */
	virtual QString getName(){return "Experiment";};
	/*! \brief Overrides UIEnabled::setName() to do nothing since the Experiment's name for scripting purposes is always
	 *	"Experiment"
	 *	\sa getName()
	 */
	virtual void setName(QString newName){};
	void setEngine(QSharedPointer<Engine::PictoEngine> engine);
	QSharedPointer<Engine::PictoEngine> getEngine();
	void addTask(QSharedPointer<Task> task);
	bool runTask(QString taskName);

	QStringList getTaskNames();
	QSharedPointer<Task> getTaskByName(QString taskName);

	virtual bool hasEditableDescendants(){return true;};
	virtual QString friendlyTypeName(){return "Experiment";};

	ASSOCIATE_ROOT_HOST_PUBLIC_IMPLEMENTATION

protected:
	virtual QString defaultTagName(){return "Experiment";};

	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);


private:
	/*! \brief Sets the input signal's xOffset.
	 *	\sa xOffset
	 */
	void setXOffset(double value) { propertyContainer_->setPropertyValue("XOffset",value); };
	/*! \brief Gets the input signal's xOffset.
	 *	\sa xOffset
	 */
	double getXOffset(){return propertyContainer_->getPropertyValue("XOffset").toDouble(); };
	/*! \brief Sets the input signal's yOffset.
	 *	\sa yOffset
	 */
	void setYOffset(double value) { propertyContainer_->setPropertyValue("YOffset",value); };
	/*! \brief Gets the input signal's yOffset.
	 *	\sa yOffset
	 */
	double getYOffset(){return propertyContainer_->getPropertyValue("YOffset").toDouble(); };
	/*! \brief Sets the input signal's xGain.
	 *	\sa xGain
	 */
	void setXGain(double value) { propertyContainer_->setPropertyValue("XGain",value); };
	/*! \brief Gets the input signal's xGain.
	 *	\sa yGain
	 */
	double getXGain(){return propertyContainer_->getPropertyValue("XGain").toDouble(); };
	/*! \brief Sets the input signal's yGain.
	 *	\sa yGain
	 */
	void setYGain(double value) { propertyContainer_->setPropertyValue("YGain",value); };
	/*! \brief Gets the input signal's yGain.
	 *	\sa yGain
	 */
	double getYGain(){return propertyContainer_->getPropertyValue("YGain").toDouble(); };
	/*! \brief Sets the input signal's xySignalShear.
	 *	\sa xySignalShear
	 */
	void setXYSignalShear(double value) { propertyContainer_->setPropertyValue("XYSignalShear",value); };
	/*! \brief Sets the input signal's xySignalShear.
	 *	\sa xySignalShear
	 */	
	double getXYSignalShear(){return propertyContainer_->getPropertyValue("XYSignalShear").toDouble(); };

	Experiment();
	QList<QSharedPointer<Task> > tasks_;
	QSharedPointer<PropertyTable> propTable_;
	QSharedPointer<Engine::PictoEngine> engine_;
	bool signalCoeffInitialized_;

	bool hostIdBeingEdited_;
private slots:
	/*! \brief Used to change this Experiment's hostId whenever something about it or a descendant is edited.
	 *	\sa AssociateRootHost
	 */
	void changeHostId(){if(hostIdBeingEdited_) return;hostIdBeingEdited_ = true;propertyContainer_->setPropertyValue("HostId",QUuid::createUuid());hostIdBeingEdited_ = false;};

	void updateSignalCoefficients(Property* changedProp,QVariant var);
	void sortTasksIntoList(QSharedPointer<Asset> newChild);
};


}; //namespace Picto

#endif
