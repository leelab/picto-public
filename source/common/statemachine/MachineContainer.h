#ifndef _MACHINECONTAINER_H
#define _MACHINECONTAINER_H

#include "../common.h"
#include "Transition.h"
#include "StateMachineElement.h"

namespace Picto {

/*!	\brief An ancestor class for StateMachineElement's that allow transitions
 *
 *
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API MachineContainer : public StateMachineElement
#else
class MachineContainer : public StateMachineElement
#endif
{
	Q_OBJECT
public:
	MachineContainer(QString transitionTag, QString elementTag);

	bool addTransition(QSharedPointer<Transition> transition);
	void addElement(QSharedPointer<ResultContainer> element);

protected:
	//virtual void elementAdded(QSharedPointer<ResultContainer> element) = 0;
	virtual void postSerialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QSharedPointer<AssetFactory> elementFactory_;
	QMap<QString, QSharedPointer<ResultContainer> > elements_;
	QMultiMap<QString, QSharedPointer<Transition> > transitions_; //<source, transition>

private:
	bool getTransitionAssets(QSharedPointer<Transition> transition, QSharedPointer<ResultContainer>& source,QSharedPointer<ResultContainer>& sourceResult, QSharedPointer<ResultContainer>& destination);
	QString transitionTag_;
	QString elementTag_;

private slots:
	void updateListsFromChildren();
};


}; //namespace Picto

#endif