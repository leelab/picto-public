#ifndef _MACHINECONTAINER_H
#define _MACHINECONTAINER_H

#include "../common.h"
#include "Transition.h"
#include "OutputElementContainer.h"

namespace Picto {

/*!	\brief An ancestor class for StateMachineElement's that allow transitions
 *
 *
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
