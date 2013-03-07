#include "MachineContainer.h"
#include "../memleakdetect.h"

using namespace Picto;

MachineContainer::MachineContainer(QString transitionTag, QString elementTag)
:
transitionTag_(transitionTag),
elementTag_(elementTag)
{
	transitionFactory_ = QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(Transition::Create)));
	AddDefinableObjectFactory(transitionTag_,transitionFactory_);
	elementFactory_ = QSharedPointer<AssetFactory>(new AssetFactory(0,-1));
	AddDefinableObjectFactory(elementTag_,elementFactory_);
	AddDefinableProperty("EntryScript","");
	AddDefinableProperty("ExitScript","");
}

//! \brief Adds a transition to this machineContainer
bool MachineContainer::addTransition(QSharedPointer<Transition> transition)
{
	QSharedPointer<ResultContainer> source,sourceResult,destination;
	if(!getTransitionAssets(transition,source,sourceResult,destination))
		return false;

	//It looks like this is a new, valid transition.
	//Set the pointers to source, sourceResult, and Destination into the 
	//transition so that it will be updated if their names change.
	//(Note that a name change would mess up our transitions_ list since it is
	//indexed by name, but that only matters in an experiment.  In a real experiment
	//the name won't ever change, so it is irrelevant.
	transition->setSource(source.staticCast<Asset>());
	transition->setSourceResult(sourceResult.staticCast<Asset>());
	transition->setDestination(destination.staticCast<Asset>());
	
	//Check if the transition is already in our child map and if not add it.
	QList<QSharedPointer<Asset>> transChildren = getGeneratedChildren("Transition");
	bool found = false;
	foreach(QSharedPointer<Asset> transChild,transChildren)
	{
		if(transChild == transition)
		{
			found = true;
			break;
		}
	}
	if(!found)
		AddChild("Transition",transition);

	//Add transition to transitions list
	transitions_.insert(transition->getSource(), transition);
	if(transition->getSource().isEmpty() && transition->getSourceResult().isEmpty())
		initTransition_ = transition;
		
	return true;
}

void MachineContainer::addElement(QSharedPointer<ResultContainer> element)
{
	//Add the element to our map
	elements_.insert(element->getName(),element);
	//addChildScriptableContainer(element);
}

void MachineContainer::postDeserialize()
{
	StateMachineElement::postDeserialize();
	updateListsFromChildren();
}

bool MachineContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!StateMachineElement::validateObject(xmlStreamReader))
		return false;

	//Confirm that all transitions are legal
	QList<QSharedPointer<Asset>> transitionList = getGeneratedChildren(transitionTag_);
	foreach(QSharedPointer<Asset> transition, transitionList)
	{
		QSharedPointer<Transition> tran = transition.staticCast<Transition>();
		QSharedPointer<ResultContainer> s,r,d;
		if(!getTransitionAssets(tran,s,r,d))
		{
			if(s.isNull() && !r.isNull())
			{
				QString errMsg = QString("Transition Container: %1 has an illegal transition.  "
					"Source: %2 is not a element.").arg(getName()).arg(tran->getSource());
				addError("MachineContainer", errMsg,xmlStreamReader);
				return false;
			}
			if(r.isNull() && !s.isNull())
			{
				QString errMsg = QString("Transition Container: %1 has an illegal transition.  "
				"Result: %2 is not a result generated by source: %3").arg(getName())
				.arg(tran->getSourceResult()).arg(tran->getSource());
				addError("MachineContainer", errMsg,xmlStreamReader);
				return false;
			}
			if(d.isNull())
			{
				QString errMsg = QString("Transition Container: %1 has an illegal transition.  "
				"Destination: %2 is not a element.").arg(getName()).arg(tran->getDestination());
				addError("MachineContainer", errMsg,xmlStreamReader);
				return false;
			}
			//If we got here and there is no source and no source result, there is more than on initial state transition.
			if(s.isNull() && r.isNull())
			{
				QString errMsg = QString(
									"MachineContainer: %1 has multiple initial state transitions.  ").arg(getName());
				addError("MachineContainer", errMsg, xmlStreamReader);
				return false;
			}
			//If we got to here, they're all valid, but the function failed.  This means that something 
			//else is already transitioning from this source result.
			QString errMsg = QString(
								"MachineContainer: %1 has multiple transitions originating from the same result of %2 on %3"
								).arg(getName()).arg(tran->getSourceResult()).arg(tran->getSource());
			addError("MachineContainer", errMsg, xmlStreamReader);
			return false;
		}
	}

	//Confirm that every element has all of its results connected
	QList<QSharedPointer<Asset>> elementList = getGeneratedChildren(elementTag_);
	foreach(QSharedPointer<Asset> elem, elementList)
	{
		QSharedPointer<ResultContainer> element = elem.staticCast<ResultContainer>();
		QList<QSharedPointer<Asset>> elemResults = element->getGeneratedChildren("Result");
		foreach(QSharedPointer<Asset> result, elemResults)
		{
			bool found = false;
			foreach(QSharedPointer<Asset> transition, transitionList)
			{
				if((transition.staticCast<Transition>()->getSource() == element->getName())
					&& (transition.staticCast<Transition>()->getSourceResult() == result->getName()))
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				QString elementName = element->getName();
				QString unconnectedResult = result->getName();
				QString errMsg = QString("MachineContainer: %1 Element: %2 has an "
					"unconnected result of %3").arg(getName()).arg(elementName).arg(unconnectedResult);
				addError("MachineContainer", errMsg, xmlStreamReader);
				return false;
			}
		}			
	}

	//Confirm that all of this MachineContainer's results are internally linked to something
	QList<QSharedPointer<Asset>> results = getGeneratedChildren("Result");
	foreach(QSharedPointer<Asset> result, results)
	{
		bool found = false;
		foreach(QSharedPointer<Asset> transition, transitionList)
		{
			if(transition.staticCast<Transition>()->getDestination() == result->getName())
			{
				found = true;
				continue;
			}
		}
		if(!found)
		{
			QString errMsg = QString("MachineContainer: %1 has an unconnected result of %2").arg(getName()).arg(result->getName());
			addError("MachineContainer", errMsg, xmlStreamReader);
			return false;
		}
	}

	//Confirm that all element and result names are unique (otherwise transition destinations can be ambiguous)
	QStringList names;
	foreach(QSharedPointer<Asset> asset,elementList)
	{
		names.push_back(asset->getName());
	}
	foreach(QSharedPointer<Asset> asset,results)
	{
		names.push_back(asset->getName());
	}
	names.sort();
	if(names.size()>0)
	{
		for(QStringList::iterator it = names.begin();it != (names.end()-1);it++)
		{
			if((*it) == (*(it+1)))
			{
				QString errMsg = QString("MachineContainer: Two components of a MachineContainer can not have the same name: \"%1\".").arg(*it);
				addError("MachineContainer", errMsg, xmlStreamReader);
				return false;
			}
		}
	}


	//If we made it this far, all the transitions are "legal"
	return true;
}

bool MachineContainer::hasScripts()
{
	return (!propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty()
		|| !propertyContainer_->getPropertyValue("ExitScript").toString().isEmpty());
}

QMap<QString,QPair<QString,QString>>  MachineContainer::getScripts()
{
	QMap<QString,QPair<QString,QString>>  scripts;
	if(!hasScripts())
		return scripts;

	if(!propertyContainer_->getPropertyValue("EntryScript").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"Entry";
		scripts[scriptName] = QPair<QString,QString>(QString(),"EntryScript");
	}
	if(!propertyContainer_->getPropertyValue("ExitScript").toString().isEmpty())
	{
		QString scriptName = getName().simplified().remove(' ')+"Exit";
		scripts[scriptName] = QPair<QString,QString>(QString(),"ExitScript");
	}
	return scripts;
}


bool MachineContainer::getTransitionAssets(QSharedPointer<Transition> transition, QSharedPointer<ResultContainer>& source, QSharedPointer<ResultContainer>& sourceResult, QSharedPointer<ResultContainer>& destination)
{
	source = sourceResult = destination = QSharedPointer<ResultContainer>();
	//Get source, sourceResult, and Destination pointers.  Use child lists instead of elements_ / transitions_ lists 
	//in case something happened post serialization (ie. in the state machine editor ) and the elements_, transitions_
	//lists weren't updated (there is no need to update them except just before running an experiment, in which case we 
	//regenerate the experiment from xml).
	//Search for source and destination elements.
	QList<QSharedPointer<Asset>> elementsList = getGeneratedChildren(elementTag_);
	foreach(QSharedPointer<Asset> element, elementsList)
	{
		if(element->getName() == transition->getSource())
		{
			source = element.staticCast<ResultContainer>();
			if(!destination.isNull())
				break;
		}
		if(element->getName() == transition->getDestination())
		{
			destination = element.staticCast<ResultContainer>();
			if(!source.isNull())
				break;
		}
	}
	
	if(destination.isNull())
	{	//Destination must be a result.  Search there.
		QList<QSharedPointer<Asset>> resultList = getGeneratedChildren("Result");
		foreach(QSharedPointer<Asset> res, resultList)
		{
			if(res->getName() == transition->getDestination())
			{
				destination = res.staticCast<ResultContainer>();
				break;
			}
		}
	}
	if(source.isNull() && !transition->getSourceResult().isEmpty())
		return false;	//If both source and sourceResult are empty, its a transition to the initial state.
	if(!transition->getSourceResult().isEmpty())
	{
		//Search for sourceResult in source.
		QList<QSharedPointer<Asset>> resultList = source->getGeneratedChildren("Result");
		foreach(QSharedPointer<Asset> res, resultList)
		{
			if(res->getName() == transition->getSourceResult())
			{
				sourceResult = res.staticCast<ResultContainer>();
				break;
			}
		}
		if(sourceResult.isNull())
			return false;
	}
	if(destination.isNull())
		return false;

	//If we made it this far, we found source, sourceResult and Destination.  
	//Make sure nothing else is transitioning from this transitions sourceResult
	foreach(QSharedPointer<Transition> trans, transitions_)
	{
		if( (trans != transition) && (trans->getSource() == transition->getSource())
			&& (trans->getSourceResult() == transition->getSourceResult())
			)
			return false;
	}
	return true;
}

void MachineContainer::updateListsFromChildren()
{
	elements_.clear();
	transitions_.clear();
	//Add ResultContainers
	QList<QSharedPointer<Asset>> elements = getGeneratedChildren(elementTag_);
	foreach(QSharedPointer<Asset> element,elements)
	{
		addElement(element.staticCast<ResultContainer>());
	}
	
	//Add ResultContainers
	QList<QSharedPointer<Asset>> transitions = getGeneratedChildren(transitionTag_);
	foreach(QSharedPointer<Asset> trans,transitions)
	{
		addTransition(trans.staticCast<Transition>());
	}

}
