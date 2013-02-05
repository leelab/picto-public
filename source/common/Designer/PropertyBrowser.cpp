#include <QtWidgets>
#include "PropertyBrowser.h"
#include "../../common/storage/datastore.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
PropertyBrowser::PropertyBrowser(QSharedPointer<EditorState> editorState,QWidget *parent) :
	editorState_(editorState),
	QtButtonPropertyBrowser(parent),
	propertyFactory_(new PropertyEditorFactory())
{
	connect(editorState_.data(), SIGNAL(selectedAssetChanged(QSharedPointer<Asset>)),
        this, SLOT(assetSelected(QSharedPointer<Asset>)));
	//connect(propertyFactory_.data(), SIGNAL(propertyChanged(QtProperty*,bool)),
 //       this, SLOT(propertyFocusChanged(QtProperty*,bool)));
	connect(propertyFactory_.data(), SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)),
        this, SLOT(propertyEdited(QSharedPointer<Property>,QVariant)));
}

void PropertyBrowser::assetSelected(QSharedPointer<Asset> asset)
{
	clear();
	propertyFactory_->clear();
	if(asset.isNull())
		return;
					
	QSharedPointer<DataStore> dataStore(asset.dynamicCast<DataStore>());
	if(dataStore.isNull())
		return;
	//if(!manager_.isNull())
	//{
	//	unsetFactoryForManager(manager_.data());
	//}





	//------------------DECOUPLE CODE----------------------------------------------------------------------
	//Build an internal property manager and use it instead of the one in the propertycontainer itself
	//as step one for property manager<->property decoupling
	propManager_ = QSharedPointer<QtVariantPropertyManager>(new QtVariantPropertyManager());
	QtVariantProperty *groupItem = propManager_->addProperty(QtVariantPropertyManager::groupTypeId(),
														  asset->getName());
	setFactoryForManager(propManager_.data(), propertyFactory_.data());

	QHash<QString, QVector<QSharedPointer<Property>>> properties = dataStore->getPropertyContainer()->getProperties();
	QStringList orderedProps = dataStore->getOrderedPropertyList();
	foreach(QString propTag,orderedProps)
	{
		QVector<QSharedPointer<Property>> propVec = properties.value(propTag);
		foreach(QSharedPointer<Property> prop,propVec)
		{
			QtVariantProperty *item = propManager_->addProperty(prop->type(),
														  prop->getName());
			item->setValue(prop->value());
			propertyFactory_->setNextProperty(prop);
			addProperty(item);
		}
	}
	//------------------DECOUPLE CODE----------------------------------------------------------------------




















	//QSharedPointer<QtVariantPropertyManager> manager = dataStore->getPropertyContainer()->getPropertyManager();
	//setFactoryForManager(manager.data(), propertyFactory_.data());
	//QHash<QString, QVector<QSharedPointer<Property>>> properties = dataStore->getPropertyContainer()->getProperties();
	//QStringList orderedProps = dataStore->getOrderedPropertyList();
	//foreach(QString propTag,orderedProps)
	//{
	//	QVector<QSharedPointer<Property>> propVec = properties.value(propTag);
	//	foreach(QSharedPointer<Property> prop,propVec)
	//	{
	//		propertyFactory_->setNextProperty(prop);
	//		addProperty(prop->getVariantProperty());
	//	}
	//}
	setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
	//setMinimumHeight(childrenRect().height());
}

void PropertyBrowser::propertyEdited(QSharedPointer<Property> prop,QVariant val)
{
	prop->setValue(val);
	editorState_->setLastActionUndoable();

	//Set the property value to its corresponding InitProperty if its scriptable
	QSharedPointer<Asset> currAsset = editorState_->getSelectedAsset();
	Q_ASSERT(currAsset);
	if(!currAsset->inherits("Picto::Scriptable"))
		return;
	QSharedPointer<Scriptable> currScriptable = currAsset.staticCast<Scriptable>();
	QSharedPointer<PropertyContainer> currInitPropContainer = currScriptable->getInitPropertyContainer();
	QSharedPointer<Property> currInitProp = currInitPropContainer->getProperty(prop->identifier());
	Q_ASSERT_X(currInitProp
		,"PropertyBrowser::propertyEdited"
		,"There is no InitProperty corresponding to the edited " + prop->identifier().toLatin1() + " property");
	currInitProp->setValue(prop->value());
}