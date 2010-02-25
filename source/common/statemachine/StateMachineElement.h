/*!	\brief A StateMachineElement is a piece that can be used in a state machine
 *	
 *	StateMachineElements would include FlowElements, States, Trials, 
 *	Stages, and States.  Not all StateMachineElements need all of
 *	the functionality provided.
 */

#ifndef _STATEMACHINEELEMENT_H_
#define _STATEMACHINEELEMENT_H_

#include "../common.h"

#include <QObject>
#include <QPoint>
#include <QRect>
#include <QSharedPointer>
#include <QStringList>

#include "../random/PictoIdentifiableObject.h"
#include "../storage/DataStore.h"

namespace Picto {

#if defined WIN32 || defined WINCE
class PICTOLIB_API StateMachineElement : public QObject, public PictoIdentifiableObject, public DataStore
#else
class StateMachineElement : public QObject, public PictoIdentifiableObject
#endif
{
	Q_OBJECT

public:
	StateMachineElement();

	//All StateMachineElements must implement a run function that returns a string
	virtual QString run() = 0;
	
	QString type() { return type_; }
	bool addTransition(QUuid source, QString condition, QUuid destination);
	bool setDefaultTransition(QUuid source, QUuid destination);
	bool setInitialElement(QUuid id);

	void setName(QString name) { name_ = name; };
	QString getName() { return name_; }

	virtual QPoint getDisplayLayoutPosition();

	//These will need to be implemented in all subclasses for the GUI
	/*virtual QRect getDisplayBoundingRect() = 0;
	virtual void draw() = 0;*/

	//DataStore functions
	virtual bool serializeAsXml(QSharedPointer<QXmlStreamWriter> xmlStreamWriter) = 0;
	virtual bool deserializeFromXml(QSharedPointer<QXmlStreamReader> xmlStreamReader) = 0;


protected:
	QString type_;	//e.g. "FlowElement", "Stage", etc
	QStringList validConatinedTypes_; //Types of elements that this element can contain
	QMap<QUuid, QMap<QString, QUuid> > transitions_;	//transition map
	QMap<QUuid, QUuid> defaultTransitions_;

	QUuid initialElement_;	//Where do we start?

	QString name_;

	QPoint layoutPosition_;


	

};


}; //namespace Picto

#endif
