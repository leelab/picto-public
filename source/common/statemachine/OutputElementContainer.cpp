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

#include "../stimuli/BoxGraphic.h"
#include "../stimuli/DiamondGraphic.h"
#include "../stimuli/ShapeShifterGraphic.h"
#include "../stimuli/TokenTrayGraphic.h"
#include "../stimuli/TokenFactoryGraphic.h"
#include "../stimuli/CircleGraphic.h"
#include "../stimuli/EllipseGraphic.h"
#include "../stimuli/ImageGraphic.h"
#include "../stimuli/GridGraphic.h"
#include "../stimuli/RandomlyFilledGridGraphic.h"
#include "../stimuli/TextGraphic.h"
#include "../stimuli/OperatorInfoGraphic.h"
#include "../stimuli/DigitalOutput.h"
#include "../stimuli/BinaryDataOutput.h"
#include "../stimuli/AudioElement.h"
#include "../stimuli/AnalogInput.h"
#include "../statemachine/ScriptFunction.h"

#include "../memleakdetect.h"

using namespace Picto;

/*! \brief Creates a new OutputElementContainer and adds lots of AssetFactory objects to it for creating all of the various types of 
 *	OutputElements.
 */
OutputElementContainer::OutputElementContainer() :
	visualElementFactory_(new AssetFactory(0,-1)),
	audioElementFactory_(new AssetFactory(0,-1)),
	outputSignalFactory_(new AssetFactory(0,-1)),
	inputSignalFactory_(new AssetFactory(0,-1)),
	scriptFunctionFactory_(new AssetFactory(0,-1))
{
	//Hmm... It may have made more sense to put ScriptFunction somewhere else, it isn't an OutputElement.
	AddDefinableObjectFactory("ScriptFunction",scriptFunctionFactory_);
	scriptFunctionFactory_->addAssetType("ScriptFunction",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ScriptFunction::Create))));

	AddDefinableObjectFactory("VisualElement",visualElementFactory_);
	//For the sake of cleanliness, we should probably have a StimulusContainer class that adds all of the following.
	//functionally, there is no problem with just adding them here.
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
	visualElementFactory_->addAssetType(TokenFactoryGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TokenFactoryGraphic::Create))));
	visualElementFactory_->addAssetType(GridGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(GridGraphic::Create))));
	visualElementFactory_->addAssetType(RandomlyFilledGridGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RandomlyFilledGridGraphic::Create))));
	visualElementFactory_->addAssetType(TextGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TextGraphic::Create))));
	visualElementFactory_->addAssetType(OperatorInfoGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(OperatorInfoGraphic::Create))));
	visualElementFactory_->addAssetType(ImageGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ImageGraphic::Create))));

	AddDefinableObjectFactory("AudioElement",audioElementFactory_);
	audioElementFactory_->addAssetType(AudioElement::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(AudioElement::Create))));

	AddDefinableObjectFactory("OutputSignal",outputSignalFactory_);
	outputSignalFactory_->addAssetType(DigitalOutput::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(DigitalOutput::Create))));

	AddDefinableObjectFactory("OutputSignal",outputSignalFactory_);
	outputSignalFactory_->addAssetType(BinaryDataOutput::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0, -1, AssetFactory::NewAssetFnPtr(BinaryDataOutput::Create))));

	AddDefinableObjectFactory("InputSignal", inputSignalFactory_);
	inputSignalFactory_->addAssetType(AnalogInput::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0, -1, AssetFactory::NewAssetFnPtr(AnalogInput::Create))));
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
