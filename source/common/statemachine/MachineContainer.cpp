#include <QRegularExpression>
#include "MachineContainer.h"
#include "../variable/StringVariable.h"
#include "../variable/NumberVariable.h"
#include "../variable/VariableList.h"
#include "../variable/VariableMap.h"
#include "../parameter/AudioFileParameter.h"
#include "../parameter/BooleanParameter.h"
#include "../parameter/ColorParameter.h"
#include "../parameter/ChoiceParameter.h"
#include "../parameter/ImageFileParameter.h"
#include "../parameter/NumericParameter.h"
#include "../parameter/RangeParameter.h"
#include "../parameter/RandomIntParameter.h"
#include "../parameter/RandomDoubleParameter.h"
#include "../parameter/DoubleParameter.h"
#include "../parameter/PseudorandomIntParameter.h"
#include "../parameter/TimerParameter.h"
#include "../parameter/OperatorClickParameter.h"
#include "../parameter/SignalValueParameter.h"
#include "../controlelements/circletarget.h"
#include "../controlelements/recttarget.h"
#include "../memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a new MachineContainer object.
 *	\details transitionTag is the name that will be used by this object for serialization of Transitions.  elementTag is
 *	the name that will be used by this object for serialization of other elements of the StateMachine.
 *	\note This system of passing in tag names for xml serializations is mostly for historical reasons, and it would be a
 *	good idea to clean this type of thing up at some point.  In general, the Factory design pattern can be improved pretty
 *	significantly in DataStores overall.
 */
MachineContainer::MachineContainer(QString transitionTag, QString elementTag)
:
parameterFactory_(new AssetFactory(0,-1)),
variableFactory_(new AssetFactory(0,-1)),
controlTargetFactory_(new AssetFactory(0,-1)),
transitionTag_(transitionTag),
elementTag_(elementTag),
addingTransition_(false)
{
	transitionFactory_ = QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(Transition::Create)));
	AddDefinableObjectFactory(transitionTag_,transitionFactory_);
	elementFactory_ = QSharedPointer<AssetFactory>(new AssetFactory(0,-1));
	AddDefinableObjectFactory(elementTag_,elementFactory_);

	AddDefinableObjectFactory("Variable",variableFactory_);
	variableFactory_->addAssetType("NumberVariable",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(NumberVariable::Create))));
	variableFactory_->addAssetType("StringVariable",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(StringVariable::Create))));
	variableFactory_->addAssetType("VariableList",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(VariableList::Create))));
	variableFactory_->addAssetType("VariableMap",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(VariableMap::Create))));

	AddDefinableObjectFactory("Parameter",parameterFactory_);
	parameterFactory_->addAssetType("Boolean",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(BooleanParameter::Create))));
	parameterFactory_->addAssetType("Color",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ColorParameter::Create))));
	parameterFactory_->addAssetType("Numeric",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(NumericParameter::Create))));
	parameterFactory_->addAssetType("Range",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RangeParameter::Create))));
	parameterFactory_->addAssetType("RandomInt",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RandomIntParameter::Create))));
	parameterFactory_->addAssetType("RandomDouble",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RandomDoubleParameter::Create))));
	parameterFactory_->addAssetType("Double",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(DoubleParameter::Create))));
	parameterFactory_->addAssetType("PseudorandomInt",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PseudorandomIntParameter::Create))));
	parameterFactory_->addAssetType("Timer",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TimerParameter::Create))));
	parameterFactory_->addAssetType("OperatorClick",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(OperatorClickParameter::Create))));
	parameterFactory_->addAssetType("SignalValue",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(SignalValueParameter::Create))));
	parameterFactory_->addAssetType("ImageFile",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ImageFileParameter::Create))));
	parameterFactory_->addAssetType("AudioFile",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AudioFileParameter::Create))));



	AddDefinableObjectFactory("ControlTarget",controlTargetFactory_);
	controlTargetFactory_->addAssetType("CircleTarget",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CircleTarget::Create))));
	controlTargetFactory_->addAssetType("RectTarget",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RectTarget::Create))));
}

/*! \brief Adds a transition to this MachineContainer's list.
*/
bool MachineContainer::addTransition(QSharedPointer<Transition> transition)
{
	QSharedPointer<ResultContainer> source,sourceResult,destination;
	if(!getTransitionAssets(transition,source,sourceResult,destination))
		return false;

	addingTransition_ = true;
	//It looks like this is a new, valid transition.
	//Set the pointers to source, sourceResult, and Destination into the 
	//transition so that it will be updated if their names change.
	//(Note that a name change would mess up our transitions_ list since it is
	//indexed by name, but that only matters in an experiment.  In a real experiment
	//the name won't ever change, so it is irrelevant.
	//We should never need to reset the asset pointers in a transition.  If the transition 
	//already has these pointers, we're probably just resetting the same pointers
	// to it, and if we must be in a copy, paste situation where two assets temporarily have the
	//same name and asset id.  In this case, we don't want the transition to move
	//from one asset source/result/dest to another so we should skip the setting as well.
	if(transition->getDestinationAsset().isNull())
	{
		transition->setSource(source.staticCast<Asset>());
		transition->setSourceResult(sourceResult.staticCast<Asset>());
		transition->setDestination(destination.staticCast<Asset>());
	}
	
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
		
	addingTransition_ = false;
	return true;
}

/*! \brief Adds the input ResultContainer element to this MachineContainer's elements_ lookup table, with the element's name as the table key.
*/
void MachineContainer::addElement(QSharedPointer<ResultContainer> element)
{
	//Add the element to our map
	elements_.insert(element->getName(),element);
	//addChildScriptableContainer(element);
}

void MachineContainer::postDeserialize()
{
	OutputElementContainer::postDeserialize();
	propertyContainer_->getProperty("UIEnabled")->setVisible(false);
	updateListsFromChildren();

	//If we add a new child after deserialization (ie. In the State Machine Editor), we'll need to update the element and transition lists.
	//To do this, we connect childAddedAfterDeserialize() and childWasAdded() which calls updateListsFromChildren().  We do 
	//it here (and not earlier) because making this connection during deserialization will create an exponential algorithm that will slow everything down.
	connect(this,SIGNAL(childAddedAfterDeserialize(QSharedPointer<Asset>)),this,SLOT(childWasAdded(QSharedPointer<Asset>)));
}

/*! \brief Extends OutputElementContainer::validateObject() to add checking for valid Transition definitions, checking that all child element's results
 *	are connected to a transition, and checking to make sure no two elements have the same name.
 */
bool MachineContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!OutputElementContainer::validateObject(xmlStreamReader))
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
				QString errMsg = QString("Contains an illegal transition.  "
					"Source: %1 is not a element.").arg(tran->getSource());
				addError(errMsg);
				return false;
			}
			if(r.isNull() && !s.isNull())
			{
				QString errMsg = QString("Contains an illegal transition.  "
				"Result: %1 is not a result generated by source: %2")
				.arg(tran->getSourceResult()).arg(tran->getSource());
				addError(errMsg);
				return false;
			}
			if(d.isNull())
			{
				QString errMsg = QString("Contains an illegal transition.  "
				"Destination: %1 is not a element.").arg(tran->getDestination());
				addError(errMsg);
				return false;
			}
			//If we got here and there is no source and no source result, there is more than on initial state transition.
			if(s.isNull() && r.isNull())
			{
				QString errMsg = QString(
									"Contains multiple initial state transitions.  ");
				addError(errMsg);
				return false;
			}
			//If we got to here, they're all valid, but the function failed.  This means that something 
			//else is already transitioning from this source result.
			QString errMsg = QString(
								"Contains multiple transitions originating from the same result of %1 on %2"
								).arg(tran->getSourceResult()).arg(tran->getSource());
			addError(errMsg);
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
				if((transition.staticCast<Transition>()->getSourceAsset() == element)
					&& (transition.staticCast<Transition>()->getSourceResultAsset() == result))
				{
					found = true;
					break;
				}
			}
			if(!found)
			{
				QString elementName = element->getName();
				QString unconnectedResult = result->getName();
				QString errMsg = QString("%1 has an "
					"unconnected result of %2").arg(elementName).arg(unconnectedResult);
				addError(errMsg);
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
			if(transition.staticCast<Transition>()->getDestinationAsset() == result)
			{
				found = true;
				continue;
			}
		}
		if(!found)
		{
			QString errMsg = QString("%1 has an unconnected result of %2").arg(getName()).arg(result->getName());
			addError(errMsg);
			return false;
		}
	}

	//Confirm that all element, and result names are unique (otherwise transition destinations can be ambiguous)
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
				addError(errMsg);
				return false;
			}
		}
	}

	//If we made it this far, all the transitions are "legal"
	return true;
}

/*! \brief Returns the source, sourceResult, and destination ResultContainer objects that are rerferenced in the input transition.
 *	\details This function looks up these Asset's according to the names stored in the Transition.  If there is some problem
 *	in finding the Assets due to invalid names or ambiguity, the function returns false.  Otherwise, the function returns true
 *	and the output data is avaiable in the reference parameters.
 */
bool MachineContainer::getTransitionAssets(QSharedPointer<Transition> transition, QSharedPointer<ResultContainer>& source, QSharedPointer<ResultContainer>& sourceResult, QSharedPointer<ResultContainer>& destination)
{
	source = sourceResult = destination = QSharedPointer<ResultContainer>();
	//Get source, sourceResult, and Destination pointers.  Use child lists instead of elements_ / transitions_ lists 
	//in case something happened post serialization (ie. in the state machine editor ) and the elements_, transitions_
	//lists weren't updated (there is no need to update them except just before running an experiment, in which case we 
	//regenerate the experiment from xml).
	//Search for source and destination elements.
	//First try to find the asset based on asset id.
	QList<QSharedPointer<Asset>> elementsList = getGeneratedChildren(elementTag_);
	foreach(QSharedPointer<Asset> element, elementsList)
	{
		if(transition->getSourceId())
		{
			if(element->getAssetId() == transition->getSourceId())
			{
				if(element->getName() == transition->getSource())
				{
					source = element.staticCast<ResultContainer>();
					if(!destination.isNull())
						break;
				}
			}
		}

		if(transition->getDestinationId())
		{
			if(element->getAssetId() == transition->getDestinationId())
			{
				if(element->getName() == transition->getDestination())
				{
					destination = element.staticCast<ResultContainer>();
					if(!source.isNull())
						break;
				}
			}
		}
	}

	if(destination.isNull() && transition->getDestinationId())
	{	//Check for result destination by assetid
		QList<QSharedPointer<Asset>> resultList = getGeneratedChildren("Result");
		foreach(QSharedPointer<Asset> res, resultList)
		{
			if(res->getAssetId() == transition->getDestinationId())
			{
				destination = res.staticCast<ResultContainer>();
				break;
			}
		}
	}

	//If we don't have both source and destination, try to get them by name
	if(source.isNull() || destination.isNull())
	{
		foreach(QSharedPointer<Asset> element, elementsList)
		{
			if(source.isNull() && (element->getName() == transition->getSource()))
			{
				source = element.staticCast<ResultContainer>();
				if(!destination.isNull())
					break;
			}
			if(destination.isNull() && (element->getName() == transition->getDestination()))
			{
				destination = element.staticCast<ResultContainer>();
				if(!source.isNull())
					break;
			}
		}

		//Destination must be a result
		if(destination.isNull())
		{
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
	}
	
	//If there's no source but there is a source result, something is wrong.  If both are empty
	//its a transition to the initial state.
	if(source.isNull() && !transition->getSourceResult().isEmpty())
		return false;
	if(!transition->getSourceResult().isEmpty())
	{
		//Search for sourceResult in source.
		//First search by Id
		if(transition->getSourceResultId())
		{
			QList<QSharedPointer<Asset>> resultList = source->getGeneratedChildren("Result");
			foreach(QSharedPointer<Asset> res, resultList)
			{
				if(res->getAssetId() == transition->getSourceResultId())
				{
					sourceResult = res.staticCast<ResultContainer>();
					break;
				}
			}
		}

		//Now try searching by name
		if(sourceResult.isNull())
		{
			QList<QSharedPointer<Asset>> resultList = source->getGeneratedChildren("Result");
			foreach(QSharedPointer<Asset> res, resultList)
			{
				if(res->getName() == transition->getSourceResult())
				{
					sourceResult = res.staticCast<ResultContainer>();
					break;
				}
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
		QString transitionSrcRealName;
		QString transSrcRealName;
		if(transition->getSourceAsset())
			transitionSrcRealName = transition->getSourceAsset()->getName();
		if(trans->getSourceAsset())
			transSrcRealName = trans->getSourceAsset()->getName();
		QString src = transition->getSource();
		QString dest = transition->getDestination();
		QString result = transition->getSourceResult();
		if( (trans != transition) && (trans->getSourceAsset() == source)
			&& (trans->getSourceResultAsset() == sourceResult)
			)
			return false;
	}
	return true;
}

/*! \brief Called when a child is added to this MachineContainer.  Calls updateListsFromChildren() to update all of the various internal lists
 *	accordingly.
 */
void MachineContainer::childWasAdded(QSharedPointer<Asset>)
{
	updateListsFromChildren();
}

/*! \brief Repopulates all internal lists (element and transition lists) according to the current child elements and their types.
*/
void MachineContainer::updateListsFromChildren()
{
	elements_.clear();
	//Add ResultContainers
	QList<QSharedPointer<Asset>> elements = getGeneratedChildren(elementTag_);
	foreach(QSharedPointer<Asset> element,elements)
	{
		addElement(element.staticCast<ResultContainer>());
	}
	
	//Since this function gets called whenever addChild is called (after postDeserialize())
	//and addTransition calls addChild in some cases, we prevent rentrant cases here.
	if(!addingTransition_)
	{
		transitions_.clear();
		//Add ResultContainers
		QList<QSharedPointer<Asset>> transitions = getGeneratedChildren(transitionTag_);
		foreach(QSharedPointer<Asset> trans,transitions)
		{
			addTransition(trans.staticCast<Transition>());
		}
	}

}
