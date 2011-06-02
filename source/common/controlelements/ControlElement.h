#ifndef _CONTROLELEMENT_H_
#define _CONTROLELEMENT_H_

#include "../common.h"

#include "../property/PropertyContainer.h"
#include "../storage/DataStore.h"
#include "../engine/PictoEngine.h"

#include <QObject>

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
class PICTOLIB_API ControlElement : public DataStore
#else
class ControlElement : public DataStore
#endif
{
	Q_OBJECT

public:
	ControlElement();

	//isDone will return true if the ControlElement has completed.
	virtual bool isDone(QSharedPointer<Engine::PictoEngine> engine) {return true;};

	//getResult returns the result from a completed ControlElement
	//if the ControlElement hasn't completed, this returns an empty string
	virtual QString getResult() {return "";};

	//called to start the controller running
	virtual void start(QSharedPointer<Engine::PictoEngine> engine){};

	void setName(QString name) { propertyContainer_->setPropertyValue("Name", name); };
	QString getName() { return propertyContainer_->getPropertyValue("Name").toString(); };

	QStringList getResultList() { return results_; };
	virtual QString assetType(){return "ControlElement";};

	//DataStore Functions
	//virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	//virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader) = 0;

protected:
	virtual QString defaultTagName(){return "ControlElement";};
	bool addResult(QString resultName);
	QStringList results_;

	bool operatorVisible_;
	bool subjectVisible_;

	bool isDone_;

};


}; //namespace Picto

#endif
