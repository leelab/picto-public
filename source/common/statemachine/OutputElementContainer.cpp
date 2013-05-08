#include <QMap>
#include <QPair>
#include <QDebug>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QAction>
#include <QTextEdit>
#include <QCoreApplication>

#include "OutputElementContainer.h"
#include "../parameter/AssociateElement.h"

//#include "../stimuli/ArrowGraphic.h"
#include "../stimuli/BoxGraphic.h"
#include "../stimuli/DiamondGraphic.h"
#include "../stimuli/ShapeShifterGraphic.h"
#include "../stimuli/TokenTrayGraphic.h"
#include "../stimuli/CircleGraphic.h"
#include "../stimuli/EllipseGraphic.h"
//#include "../stimuli/LineGraphic.h"
#include "../stimuli/GridGraphic.h"
//#include "../stimuli/PictureGraphic.h"
#include "../stimuli/RandomlyFilledGridGraphic.h"
#include "../stimuli/TextGraphic.h"
#include "../stimuli/OperatorInfoGraphic.h"
#include "../stimuli/DigitalOutput.h"
#include "../stimuli/BinaryDataOutput.h"
#include "../stimuli/AudioElement.h"
#include "../statemachine/ScriptFunction.h"
#include "../memleakdetect.h"

using namespace Picto;

OutputElementContainer::OutputElementContainer() :
	visualElementFactory_(new AssetFactory(0,-1)),
	audioElementFactory_(new AssetFactory(0,-1)),
	outputSignalFactory_(new AssetFactory(0,-1)),
	scriptFunctionFactory_(new AssetFactory(0,-1))
{
	AddDefinableObjectFactory("ScriptFunction",scriptFunctionFactory_);
	scriptFunctionFactory_->addAssetType("ScriptFunction",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ScriptFunction::Create))));

	AddDefinableObjectFactory("VisualElement",visualElementFactory_);
	//For the sake of cleanliness, we should probably have a StimulusContainer class that adds all of the following.
	//functionally, there is no problem with just adding them here.
	//visualElementFactory_->addAssetType(ArrowGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ArrowGraphic::Create))));
	visualElementFactory_->addAssetType(BoxGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(BoxGraphic::Create))));
	visualElementFactory_->addAssetType(DiamondGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(DiamondGraphic::Create))));
	visualElementFactory_->addAssetType(CircleGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CircleGraphic::Create))));
	visualElementFactory_->addAssetType(EllipseGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(EllipseGraphic::Create))));
	visualElementFactory_->addAssetType(ShapeShifterGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ShapeShifterGraphic::Create))));
	visualElementFactory_->addAssetType(TokenTrayGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TokenTrayGraphic::Create))));
	//visualElementFactory_->addAssetType(LineGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LineGraphic::Create))));
	visualElementFactory_->addAssetType(GridGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(GridGraphic::Create))));
	//visualElementFactory_->addAssetType(PictureGraphic::type,
	//	QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PictureGraphic::Create))));
	visualElementFactory_->addAssetType(RandomlyFilledGridGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RandomlyFilledGridGraphic::Create))));
	visualElementFactory_->addAssetType(TextGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TextGraphic::Create))));
	visualElementFactory_->addAssetType(OperatorInfoGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(OperatorInfoGraphic::Create))));

	AddDefinableObjectFactory("AudioElement",audioElementFactory_);
	audioElementFactory_->addAssetType("AudioElement",
		QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(AudioElement::Create))));

	AddDefinableObjectFactory("OutputSignal",outputSignalFactory_);
	outputSignalFactory_->addAssetType("DigitalOutput",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(DigitalOutput::Create))));

	AddDefinableObjectFactory("OutputSignal",outputSignalFactory_);
	outputSignalFactory_->addAssetType("BinaryDataOutput",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(BinaryDataOutput::Create))));
}

/*! \brief Copies the OutputElements from the passed in container
 *	
 *	This is used when adding an child element to a parent OutputElementContainer.  By passing
 *	in the parent OutputElement container, we now have access to all 
 *	of the parent's OutputElements.  This allows us to
 *	have OutputElements with Picto scope.
 */
void OutputElementContainer::addOutputElements(QSharedPointer<OutputElementContainer> outputElementContainer)
{
	addOutputElements(outputElementContainer.data());
}

void OutputElementContainer::addOutputElements(OutputElementContainer *outputElementContainer)
{
	QList<QSharedPointer<OutputElement>> OutputElements = outputElementContainer->getOutputElementList();
	foreach(QSharedPointer<OutputElement> OutputElement,OutputElements)
	{
		addOutputElement(OutputElement);
	}
}

void OutputElementContainer::addChildOutputElementContainer(QSharedPointer<OutputElementContainer> child)
{
	outputElementContainers_.push_back(child);
	child->addOutputElements(this);
}

//! \brief Returns a list of all contained parameters
QList<QSharedPointer<OutputElement>> OutputElementContainer::getOutputElementList()
{
	return outputElements_;
}

void OutputElementContainer::ClearAssociateChildren(QUuid associateId)
{
	StateMachineElement::ClearAssociateChildren(associateId);

	//Go through the outputelements list and remove all associate children with the input associate id.
	bool somethingWasRemoved = false;
	AssociateElement* associateElem;
	for(QList<QSharedPointer<OutputElement>>::iterator iter = outputElements_.begin();iter!=outputElements_.end();)
	{
		associateElem = dynamic_cast<AssociateElement*>(iter->data());
		if(!associateElem)
		{
			iter++;
			continue;
		}
		if(associateElem->getAssociateId() == associateId)
		{
			iter = outputElements_.erase(iter);
			somethingWasRemoved = true;
			continue;
		}
		iter++;
	}
}

void OutputElementContainer::postDeserialize()
{
	StateMachineElement::postDeserialize();
	QStringList childTags = getDefinedChildTags();
	foreach(QString childTag,childTags)
	{
		QList<QSharedPointer<Asset>> tagChildren = getGeneratedChildren(childTag);
		foreach(QSharedPointer<Asset> tagChild,tagChildren)
		{
			addChildToElementLists(tagChild);
		}
	}
	connect(this,SIGNAL(childAddedAfterDeserialize(QSharedPointer<Asset>)),this,SLOT(addChildToElementLists(QSharedPointer<Asset>)));
}

bool OutputElementContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!StateMachineElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

void OutputElementContainer::addOutputElement(QSharedPointer<OutputElement> outputElement)
{
	//If the OutputElement is empty return
	if(outputElement.isNull())
		return;
	//Add all visual elements to the list whether they are part of the experiment or analysis
	outputElements_.push_back(outputElement);

	foreach(QSharedPointer<OutputElementContainer> child,outputElementContainers_)
	{
		child->addOutputElement(outputElement);
	}
}

//If a child is added to this element after postDeserialize then we may need to add it
//using addOutputElement() or addChildOutputElementContainer().  We handle that here.
void OutputElementContainer::addChildToElementLists(QSharedPointer<Asset> newChild)
{
	if(newChild.dynamicCast<OutputElement>())
		addOutputElement(newChild.staticCast<OutputElement>());
	if(newChild.dynamicCast<OutputElementContainer>())
		addChildOutputElementContainer(newChild.staticCast<OutputElementContainer>());

}