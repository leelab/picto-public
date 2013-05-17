#ifndef _CONTROLELEMENT_H_
#define _CONTROLELEMENT_H_

#include <QObject>

#include "../common.h"

#include "../property/PropertyContainer.h"
#include "../storage/DataStore.h"
#include "../engine/PictoEngine.h"
#include "../statemachine/result.h"
#include "../statemachine/resultcontainer.h"


namespace Picto {

/*! \brief Controls the exit from a scene
 *
 *	All Picto scenes must include a Control Element of some sort.  The control
 *	element determines when the scene ends.  During the scene's rendering loop,
 *	the isDone() function is called every frame (until it returns true).  Then the
 *	getResult() function is called to determine the result of the ControlElements
 *	execution(e.g. "Broke fixation", "Selected traget 3", etc)
 *
 *	To keep things simple, the behavior of a Target Controller is defined through a 
 *	set of properties stored in a property container.
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API ControlElement : public ResultContainer
#else
class ControlElement : public ResultContainer
#endif
{
	Q_OBJECT

public:
	ControlElement();
	virtual ~ControlElement(){};

	//isDone will return true if the ControlElement has completed.
	virtual bool isDone(QSharedPointer<Engine::PictoEngine> engine) {return true;};

	using ResultContainer::getResult;
	//getResult returns the result from a completed ControlElement
	//if the ControlElement hasn't completed, this returns an empty result pointer
	virtual QSharedPointer<Result> getResult() {return QSharedPointer<Result>();};

	//called to start the controller running
	virtual void start(QSharedPointer<Engine::PictoEngine> engine){};
	virtual void stop(QSharedPointer<Engine::PictoEngine> engine){};

	//Activates all Control Targets managed by this Control Element
	virtual void activateTargets(){};
	virtual void deactivateTargets(){};

	//void setName(QString name) { propertyContainer_->setPropertyValue("Name", name); };

	//QStringList getResultList() { return results_.keys(); };
	//QSharedPointer<Result> getResult(QString name);
	virtual QString getUITemplate(){return "ControlElement";};
	virtual QString friendlyTypeName(){return "Control Element";};
	virtual QString getUIGroup(){return "State Machine Elements";};

	//DataStore Functions
	//virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	//virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader) = 0;

protected:
	virtual QString defaultTagName(){return "Control Element";};
	//bool addResult(QSharedPointer<Result> result);
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

	//QMap<QString,QSharedPointer<Result>> results_;

	bool isDone_;

};


}; //namespace Picto

#endif
