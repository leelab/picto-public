#include "ParameterContainer.h"
#include "ParameterFactory.h"
#include "BooleanParameter.h"
#include "ChoiceParameter.h"
#include "NumericParameter.h"
#include "RangeParameter.h"
#include "RandomIntParameter.h"
#include "PseudorandomIntParameter.h"

#include "../stimuli/ArrowGraphic.h"
#include "../stimuli/BoxGraphic.h"
#include "../stimuli/CircleGraphic.h"
#include "../stimuli/EllipseGraphic.h"
#include "../stimuli/LineGraphic.h"
#include "../stimuli/PictureGraphic.h"
#include "../stimuli/RandomlyFilledGridGraphic.h"
#include "../stimuli/TextGraphic.h"
#include "../stimuli/AudioElement.h"
#include "../controlelements/circletarget.h"
#include "../controlelements/recttarget.h"

namespace Picto {

ParameterContainer::ParameterContainer()
{
	scriptableFactory_->addAssetType("Boolean",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(BooleanParameter::Create))));
	scriptableFactory_->addAssetType("Choice",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ChoiceParameter::Create))));
	scriptableFactory_->addAssetType("Numeric",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(NumericParameter::Create))));
	scriptableFactory_->addAssetType("Range",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RangeParameter::Create))));
	scriptableFactory_->addAssetType("RandomInt",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RandomIntParameter::Create))));
	scriptableFactory_->addAssetType("PseudorandomInt",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PseudorandomIntParameter::Create))));


	//For the sake of cleanliness, we should probably have a StimulusContainer class that adds all of the following.
	//functionally, there is no problem with just adding them here.
	scriptableFactory_->addAssetType(ArrowGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ArrowGraphic::Create))));
	scriptableFactory_->addAssetType(BoxGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(BoxGraphic::Create))));
	scriptableFactory_->addAssetType(CircleGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CircleGraphic::Create))));
	scriptableFactory_->addAssetType(EllipseGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(EllipseGraphic::Create))));
	scriptableFactory_->addAssetType(LineGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(LineGraphic::Create))));
	scriptableFactory_->addAssetType(PictureGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(PictureGraphic::Create))));
	scriptableFactory_->addAssetType(RandomlyFilledGridGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RandomlyFilledGridGraphic::Create))));
	scriptableFactory_->addAssetType(TextGraphic::type,
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(TextGraphic::Create))));
	scriptableFactory_->addAssetType("AudioElement",
		QSharedPointer<AssetFactory>(new AssetFactory(1,1,AssetFactory::NewAssetFnPtr(AudioElement::Create))));
	scriptableFactory_->addAssetType("CircleTarget",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(CircleTarget::Create))));
	scriptableFactory_->addAssetType("RectTarget",
		QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(RectTarget::Create))));
}

//QSharedPointer<Parameter> ParameterContainer::getParameter(QString name)
//{
//	return getScriptable(name).staticCast<Parameter>();
//}

void ParameterContainer::postSerialize()
{
	ResultContainer::postSerialize();
}

bool ParameterContainer::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!ResultContainer::validateObject(xmlStreamReader))
		return false;
	return true;
}

}; //namespace Picto
