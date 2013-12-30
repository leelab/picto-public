#ifndef _MACHINECONTAINER_H
#define _MACHINECONTAINER_H

#include "../common.h"
#include "Transition.h"
#include "OutputElementContainer.h"

namespace Picto {

/*!	\brief A base class for everything that can contain transitions and other elements.
 *
 *	\details This class should be refactored.  It appears the way it does mainly for historical reasons, but
 *	essentially what it does is manage transitions and objects in the StateMachine that are not StateMachineElements
 *	or OutputElements.  There should probably be one class that handles Transitions, and one or more that handles Variables,
 *  Parameters, etc.
 *	
 *	As it stands however, this is a fairly important class that handles a lot of the code necessary for elements that contain
 *	"machines" that includes some form of state transfer.  Currently these include StateMachine and State.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API MachineContainer : public OutputElementContainer
#else
class MachineContainer : public OutputElementContainer
#endif
{
	Q_OBJECT
public:
	MachineContainer(QString transitionTag, QString elementTag);
	virtual ~MachineContainer(){};

	bool addTransition(QSharedPointer<Transition> transition);
	void addElement(QSharedPointer<ResultContainer> element);

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QSharedPointer<AssetFactory> elementFactory_;
	QSharedPointer<AssetFactory> transitionFactory_;
	QSharedPointer<AssetFactory> controlTargetFactory_;
	QSharedPointer<AssetFactory> variableFactory_;
	QSharedPointer<AssetFactory> parameterFactory_;
	QMap<QString, QSharedPointer<ResultContainer> > elements_;
	QMultiMap<QString, QSharedPointer<Transition> > transitions_; //<source, transition>
	QSharedPointer<Transition> initTransition_;

private:
	bool getTransitionAssets(QSharedPointer<Transition> transition, QSharedPointer<ResultContainer>& source,QSharedPointer<ResultContainer>& sourceResult, QSharedPointer<ResultContainer>& destination);
	QString transitionTag_;
	QString elementTag_;
	bool addingTransition_;

private slots:
	void childWasAdded(QSharedPointer<Asset> newChild);
	void updateListsFromChildren();
};


}; //namespace Picto

#endif
